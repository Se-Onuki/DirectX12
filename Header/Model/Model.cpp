#include "Model.h"
#include "../Create/Create.h"
#include "../Math/Math.hpp"
#include <array>
#include "../../DirectBase/Base/DirectXCommon.h"

Model::Model()
{
}

Model::~Model()
{
	for (auto &mesh : meshList_) {
		delete mesh;
	}
}

void Model::LoadObjFile(const std::string &directoryPath, const std::string &fileName) {

#pragma region 1. ファイルを開く

	std::ifstream file{ directoryPath + "/" + fileName };
	if (!file.is_open()) return;		// 開けなかった場合、処理を終了する

#pragma endregion

	name_ = fileName.substr(0, fileName.size() - 4);

#pragma region 2. 中で必要になる変数の宣言

	meshList_.emplace_back(new Mesh);

	Mesh &modelData = *meshList_.back();				// 構築するModelData
	std::vector<Vector4> positionList;	// 位置
	std::vector<Vector3> normalList;	// 法線
	std::vector<Vector2> texCoordList;	// テクスチャ座標

	std::string line;					// ファイルから一行を格納する

#pragma endregion

#pragma region 3. ファイルを読み, ModelDataを構築

	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s{ line };
		s >> identifier;

		if (identifier == "v") {
			Vector4 position;
			s >> position.x >> position.y >> position.z;
			position.z *= -1;
			position.w = 1.f;
			positionList.push_back(position);

		}
		else if (identifier == "vt") {
			Vector2 texCoord;
			s >> texCoord.x >> texCoord.y;
			texCoord.y = 1.f - texCoord.y;
			texCoordList.push_back(texCoord);
		}
		else if (identifier == "vn") {
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			normal.z *= -1;
			normalList.push_back(normal);
		}
		else if (identifier == "f") {
			// 面は三角形限定。その他は未対応
			std::array<Mesh::VertexData, 3u> triangle = {};
			for (uint32_t faceVertex = 0u; faceVertex < 3u; ++faceVertex) {
				std::string vertexDefinition;
				s >> vertexDefinition;
				// 頂点の要素へのIndexは「 位置 / UV / 法線 」 で格納されているので、分解してIndex を取得する
				std::istringstream v{ vertexDefinition };
				uint32_t elementIndices[3];
				for (uint32_t element = 0; element < 3; ++element) {
					std::string index;
					std::getline(v, index, '/'); // 区切りでインデックスを読んでいく
					elementIndices[element] = std::stoi(index);
				}
				// 要素へのIndexから、実際の要素の値を取得して、頂点を機築する
				Vector4 position = positionList[elementIndices[0] - 1];
				Vector2 texCoord = texCoordList[elementIndices[1] - 1];
				Vector3 normal = normalList[elementIndices[2] - 1];
				// 末尾から順に(法線の逆転)
				triangle[2u - faceVertex] = { position,texCoord,normal };
			}
			// イテレータを用いた末尾への直接構築
			modelData.AddVertex(triangle[0]);
			modelData.AddVertex(triangle[1]);
			modelData.AddVertex(triangle[2]);
			/*
			modelData.vertices_.insert(modelData.vertices_.end(), triangle.begin(), triangle.end());
			const uint32_t indexOffset = (uint32_t)modelData.vertices_.size() - 3u;
			modelData.indexs_.insert(modelData.indexs_.end(), { indexOffset ,indexOffset + 1,indexOffset + 2 });*/
		}
		else if (identifier == "mtllib") {

			std::string materialFile;
			s >> materialFile;
			modelData.material_ = Material::LoadFile(directoryPath, materialFile);
		}
	}
#pragma endregion


}

void Model::Draw(ID3D12GraphicsCommandList *const commandList, const Transform &transform, const Matrix4x4 &viewProjection) const
{

	commandList->IASetVertexBuffers(0, 1, &meshList_[0]->vbView_);
	commandList->IASetIndexBuffer(&meshList_[0]->ibView_);
	commandList;
	transform;
	viewProjection;
}

void Mesh::CreateBuffer()
{
	ID3D12Device *const device = DirectXCommon::GetInstance()->GetDevice();

	HRESULT hr = S_FALSE;

#pragma region 頂点バッファ

	vertexBuff_ = CreateBufferResource(device, sizeof(VertexData) * vertices_.size());

	// リソースの先頭のアドレスから使う
	vbView_.BufferLocation = vertexBuff_->GetGPUVirtualAddress();
	// 使用するリソースの全体のサイズ
	vbView_.SizeInBytes = static_cast<UINT>(sizeof(VertexData) * vertices_.size());
	// 1頂点あたりのサイズ
	vbView_.StrideInBytes = sizeof(VertexData);


	// 頂点バッファへのデータ転送
	VertexData *vertexMap = nullptr;
	hr = vertexBuff_->Map(0, nullptr, (void **)&vertexMap);
	if (SUCCEEDED(hr)) {
		std::copy(vertices_.begin(), vertices_.end(), vertexMap);
		//vertexBuff_->Unmap(0, nullptr);
	}

#pragma endregion

	hr = S_FALSE;

#pragma region インデックスバッファ

	indexBuff_ = CreateBufferResource(device, sizeof(uint32_t) * indexs_.size());

	// リソースの先頭のアドレスから使う
	ibView_.BufferLocation = indexBuff_->GetGPUVirtualAddress();
	// 使用するリソースの全体のサイズ
	ibView_.SizeInBytes = static_cast<UINT>(sizeof(uint32_t) * indexs_.size());
	// 1添え字あたりの型情報
	ibView_.Format = DXGI_FORMAT_R32_UINT;


	// 頂点バッファへのデータ転送
	uint32_t *indexMap = nullptr;
	hr = indexBuff_->Map(0, nullptr, (void **)&indexMap);
	if (SUCCEEDED(hr)) {
		std::copy(indexs_.begin(), indexs_.end(), indexMap);
		//indexBuff_->Unmap(0, nullptr);
	}

#pragma endregion

}

void Mesh::AddVertex(const VertexData &vertex)
{
	auto it = std::find(vertices_.begin(), vertices_.end(), vertex);
	if (it != vertices_.end()) {
		indexs_.push_back((uint32_t)std::distance(vertices_.begin(), it));
	}
	else {
		vertices_.push_back(vertex);
		indexs_.push_back((uint32_t)vertices_.size() - 1u);
	}
}

void Mesh::CreateSphere(VertexData *const vertex, ID3D12Resource *const indexResource, const uint32_t &subdivision)
{

	const float kLonEvery = 360u * Angle::Dig2Rad / subdivision; // 経度
	const float kLatEvery = 180u * Angle::Dig2Rad / subdivision; // 緯度

	// 緯度の方向に分解 -90 ~ 90
	for (uint32_t latIndex = 0u; latIndex < subdivision; ++latIndex) {
		const float lat = static_cast<float>(-Angle::PI) / 2.f + kLatEvery * latIndex;
		// 経度の方向に分解 0 ~ 360
		for (uint32_t lonIndex = 0u; lonIndex < subdivision; ++lonIndex) {
			uint32_t indexStart = (latIndex * subdivision + lonIndex) * 4;
			const float lon = lonIndex * kLonEvery;

#pragma region Vertex[0] 左下

			VertexData *vertexData = &vertex[indexStart + 0u];
			vertexData->position = { //左下
				std::cos(lat) * std::cos(lon),std::sin(lat),
				std::cos(lat) * std::sin(lon), 1
			};
			vertexData->texCoord = {
				static_cast<float>(lonIndex) / subdivision,
				1.f - static_cast<float>(latIndex) / subdivision
			};
			vertexData->normal.x = vertexData->position.x;
			vertexData->normal.y = vertexData->position.y;
			vertexData->normal.z = vertexData->position.z;
			vertexData = nullptr;

#pragma endregion

#pragma region Vertex[1] 左上

			vertexData = &vertex[indexStart + 1u];
			vertexData->position = { //左上
				std::cos(lat + kLatEvery) * std::cos(lon),
				std::sin(lat + kLatEvery),
				std::cos(lat + kLatEvery) * std::sin(lon), 1
			};
			vertexData->texCoord = {
				static_cast<float>(lonIndex) / subdivision,
				1.f - static_cast<float>(latIndex + 1u) / subdivision
			};
			vertexData->normal.x = vertexData->position.x;
			vertexData->normal.y = vertexData->position.y;
			vertexData->normal.z = vertexData->position.z;
			vertexData = nullptr;

#pragma endregion

#pragma region Vertex[2] 右下

			vertexData = &vertex[indexStart + 2u];
			vertexData->position = { //右下
				std::cos(lat) * std::cos(lon + kLonEvery),
				std::sin(lat),
				std::cos(lat) * std::sin(lon + kLonEvery), 1
			};
			vertexData->texCoord = {
				static_cast<float>(lonIndex + 1u) / subdivision,
				1.f - static_cast<float>(latIndex) / subdivision
			};
			vertexData->normal.x = vertexData->position.x;
			vertexData->normal.y = vertexData->position.y;
			vertexData->normal.z = vertexData->position.z;
			vertexData = nullptr;

#pragma endregion
			//
			//#pragma region Vertex[3] 左上
			//
			//			vertexData = &vertex[indexStart + 3u];
			//			vertexData->position = { //左上
			//				std::cos(lat + kLatEvery) * std::cos(lon),
			//				std::sin(lat + kLatEvery),
			//				std::cos(lat + kLatEvery) * std::sin(lon), 1
			//			};
			//			vertexData->texCoord = {
			//				static_cast<float>(lonIndex) / subdivision,
			//				1.f - static_cast<float>(latIndex + 1u) / subdivision
			//			};
			//			vertexData->normal.x = vertexData->position.x;
			//			vertexData->normal.y = vertexData->position.y;
			//			vertexData->normal.z = vertexData->position.z;
			//			vertexData = nullptr;
			//
			//#pragma endregion

#pragma region Vertex[3] 右上

			vertexData = &vertex[indexStart + 3u];
			vertexData->position = { //右上
				std::cos(lat + kLatEvery) * std::cos(lon + kLonEvery),
				std::sin(lat + kLatEvery),
				std::cos(lat + kLatEvery) * std::sin(lon + kLonEvery), 1
			};
			vertexData->texCoord = {
				static_cast<float>(lonIndex + 1u) / subdivision,
				1.f - static_cast<float>(latIndex + 1u) / subdivision
			};
			vertexData->normal.x = vertexData->position.x;
			vertexData->normal.y = vertexData->position.y;
			vertexData->normal.z = vertexData->position.z;
			vertexData = nullptr;

#pragma endregion
			//
			//#pragma region Vertex[5] 右下
			//
			//			vertexData = &vertex[indexStart + 5u];
			//			vertexData->position = { //右下
			//				std::cos(lat) * std::cos(lon + kLonEvery),
			//				std::sin(lat),
			//				std::cos(lat) * std::sin(lon + kLonEvery), 1
			//			};
			//			vertexData->texCoord = {
			//				static_cast<float>(lonIndex + 1u) / subdivision,
			//				1.f - static_cast<float>(latIndex) / subdivision
			//			};
			//			vertexData->normal.x = vertexData->position.x;
			//			vertexData->normal.y = vertexData->position.y;
			//			vertexData->normal.z = vertexData->position.z;
			//			vertexData = nullptr;
			//
			//#pragma endregion

		}
	}

	uint32_t *indexData =
		nullptr;
	indexResource->Map(0, nullptr, reinterpret_cast<void **>(&indexData));
	for (uint32_t i = 0; i < subdivision * subdivision; i++) {

		indexData[i * 6u + 0u] = i * 4u + 0u; indexData[i * 6u + 1u] = i * 4u + 1u; indexData[i * 6u + 2u] = i * 4u + 2u;
		indexData[i * 6u + 3u] = i * 4u + 1u; indexData[i * 6u + 4u] = i * 4u + 3u; indexData[i * 6u + 5u] = i * 4u + 2u;

	}

}
