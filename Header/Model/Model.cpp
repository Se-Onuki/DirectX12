#include "Model.h"
#include "../Create/Create.h"

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
			Render::VertexData triangle[3] = {};
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
			modelData.vertices_.insert(modelData.vertices_.end(), triangle, triangle + 3);
			/*modelData.indexs.push_back(modelData.vertices.size() - 3);
			modelData.indexs.push_back(modelData.vertices.size() - 2);
			modelData.indexs.push_back(modelData.vertices.size() - 1);*/
		}
		else if (identifier == "mtllib") {

			std::string materialFile;
			s >> materialFile;
			modelData.material_ = Material::LoadFile(directoryPath, materialFile);
		}
	}
#pragma endregion


}

void Mesh::CreateBuffer()
{
	//CreateBufferResource(device, sizeof(Render::VertexData) * vertices_.size());

	// リソースの先頭のアドレスから使う
	vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	// 使用するリソースの全体のサイズ
	vbView_.SizeInBytes = static_cast<UINT>(sizeof(Render::VertexData) * vertices_.size());
	// 1頂点あたりのサイズ
	vbView_.StrideInBytes = sizeof(Render::VertexData);


	// 頂点バッファへのデータ転送
	Render::VertexData *vertMap = nullptr;
	HRESULT result = vertBuff_->Map(0, nullptr, (void **)&vertMap);
	if (SUCCEEDED(result)) {
		std::copy(vertices_.begin(), vertices_.end(), vertMap);
		vertBuff_->Unmap(0, nullptr);
	}
}
