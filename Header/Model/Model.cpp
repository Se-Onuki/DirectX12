#include "Model.h"
#include "../Create/Create.h"
#include "../Math/Math.hpp"
#include <array>
#include "../../DirectBase/Base/DirectXCommon.h"
#include "../../DirectBase/3D/ViewProjection/ViewProjection.h"
#include "../Math/Transform.h"
#include "../../DirectBase/Base/TextureManager.h"
#include "../../DirectBase/Base/Shader.h"

ID3D12GraphicsCommandList *Model::commandList_ = nullptr;
const char *const Model::defaultDirectory = "resources/";
std::array<Microsoft::WRL::ComPtr<ID3D12PipelineState>, 2u> Model::graphicsPipelineState_ = { nullptr };
Microsoft::WRL::ComPtr<ID3D12RootSignature> Model::rootSignature_ = nullptr;

void Model::StaticInit() {
	CreatePipeLine();
}

void Model::CreatePipeLine() {

	HRESULT hr = S_FALSE;
	ID3D12Device *const device = DirectXCommon::GetInstance()->GetDevice();

#pragma region PSO(Pipeline State Object)

#pragma region RootSigneture(ルートシグネチャ)

	// RootSignature生成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

#pragma region RootParameter

	// RootParameter作成
	D3D12_ROOT_PARAMETER rootParameters[5] = {};

	rootParameters[(uint32_t)Model::RootParameter::kWorldTransform].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		// CBVを使う
	rootParameters[(uint32_t)Model::RootParameter::kWorldTransform].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;	// VertexShaderで使う
	rootParameters[(uint32_t)Model::RootParameter::kWorldTransform].Descriptor.ShaderRegister = 0;						// レジスタ番号0とバインド (b0が設定されているので0)

	rootParameters[(uint32_t)Model::RootParameter::kViewProjection].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		// CBVを使う
	rootParameters[(uint32_t)Model::RootParameter::kViewProjection].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;	// VertexShaderで使う
	rootParameters[(uint32_t)Model::RootParameter::kViewProjection].Descriptor.ShaderRegister = 1;						// レジスタ番号1とバインド 


	rootParameters[(uint32_t)Model::RootParameter::kMaterial].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		// CBVを使う
	rootParameters[(uint32_t)Model::RootParameter::kMaterial].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;		// PixelShaderで使う
	rootParameters[(uint32_t)Model::RootParameter::kMaterial].Descriptor.ShaderRegister = 0;						// レジスタ番号0とバインド (b0が設定されているので0)

	rootParameters[(uint32_t)Model::RootParameter::kLight].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		// CBVを使う
	rootParameters[(uint32_t)Model::RootParameter::kLight].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;		// PixelShaderで使う
	rootParameters[(uint32_t)Model::RootParameter::kLight].Descriptor.ShaderRegister = 1;						// レジスタ番号1とバインド 

#pragma region Texture

	// DescriptorRangeの設定
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;	// 0から始める
	descriptorRange[0].NumDescriptors = 1;		// 数は1つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;	// SRVを使う
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;	// Offsetを自動計算

	rootParameters[(uint32_t)Model::RootParameter::kTexture].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;		// DescriptorTableを使う
	rootParameters[(uint32_t)Model::RootParameter::kTexture].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;					// PixelShaderで使う
	rootParameters[(uint32_t)Model::RootParameter::kTexture].DescriptorTable.pDescriptorRanges = descriptorRange;				// Tableの中身の配列を指定
	rootParameters[(uint32_t)Model::RootParameter::kTexture].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);	// Tableで使用する数

#pragma endregion

	descriptionRootSignature.pParameters = rootParameters;					// ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters);		// 配列の長さ

#pragma endregion

#pragma region Samplerの設定

	D3D12_STATIC_SAMPLER_DESC staticSamplers[] = {
	{
		.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR,			// バイナリフィルタ
		.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP,		// 0~1の範囲外をリピート
		.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER,		// 比較しない
		.MaxLOD = D3D12_FLOAT32_MAX,						// ありったけのMipMapを使う
		.ShaderRegister = 0,								// レジスタ番号 0を使う
		.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL,	// PixelShaderで使う
	}
	};
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

#pragma endregion

	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, signatureBlob.GetAddressOf(), errorBlob.GetAddressOf());
	if (FAILED(hr)) {
		DirectXCommon::Log(reinterpret_cast<char *>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	// バイナリを元に作成
	//Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	hr = device->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(rootSignature_.GetAddressOf()));
	assert(SUCCEEDED(hr));

#pragma endregion

#pragma region InputLayout(インプットレイアウト)

	// InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};

	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);

#pragma endregion

#pragma region BlendState(ブレンドステート)

	// BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	// 全ての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

#pragma endregion

#pragma region RasterizerState(ラスタライザステート)

	// RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	// 三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

#pragma endregion

#pragma region ShaderをCompileする

#pragma region Shader

	Shader vertexShader = Shader::Compile(L"Object3D.VS.hlsl", L"vs_6_0");
	Shader pixelShader = Shader::Compile(L"Object3D.PS.hlsl", L"ps_6_0");

#pragma endregion

#pragma endregion

#pragma region DepthStencilState

#pragma region DepthStencileStateの設定

	// DepthStencileStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を有効にする
	depthStencilDesc.DepthEnable = true;
	// 書き込みします。
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	// 比較関数はLessEqual。つまり、近ければ表示される。
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

#pragma endregion

#pragma endregion


#pragma region PSOを生成する

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature_.Get();		// RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;			// InputLayout
	graphicsPipelineStateDesc.VS = vertexShader.GetBytecode();			// VertexShader
	graphicsPipelineStateDesc.PS = pixelShader.GetBytecode();			// PixelShader
	graphicsPipelineStateDesc.BlendState = blendDesc;					// BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;			// RasterizeState

	// DSVのFormatを設定する
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	// 利用するトポロジ(形状)のタイプ。三角形。
	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// どのように画面に色を打ち込むかの設定(気にしなくても良い)
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// 実際に生成
	hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState_[0]));
	assert(SUCCEEDED(hr));

#pragma endregion


#pragma region BlendState(ブレンドステート)

	// 全ての色要素を書き込む
	//blendDesc.RenderTarget[0].RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	graphicsPipelineStateDesc.BlendState = blendDesc;

	// 実際に生成
	hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState_[1]));
	assert(SUCCEEDED(hr));


#pragma endregion


#pragma endregion

}

void Model::LoadMtlFile(const std::string &directoryPath, const std::string &fileName) {

	Material *materialData = nullptr;
	std::string line;

	std::ifstream file{ Model::defaultDirectory + directoryPath + fileName };
	if (!file.is_open()) return;		// 開けなかった場合、処理を終了する

#pragma region ファイルからMaterialDataを構築

	while (std::getline(file, line)) {

		std::string identifier;
		std::istringstream s{ line };
		s >> identifier;

		// 新規マテリアルの生成
		if (identifier == "newmtl") {
			std::string materialName;
			s >> materialName;
			materialMap_[materialName].reset(new Material{ materialName });
			materialData = materialMap_[materialName].get();
			materialData->CreateBuffer();
		}
		else if (identifier == "Kd") {
			if (materialData && materialData->mapData_) {
				Vector4 &color = materialData->mapData_->color;
				s >> color.x >> color.y >> color.z;
			}
		}

		else if (identifier == "map_Kd") {

			std::string token;
			Transform uv{};
			uv.scale = Vector3::one();

			while (s >> token) {
				if (token == "-o") {
					s >> uv.translate.x >> uv.translate.y >> uv.translate.z;
				}
				else if (token == "-s") {
					s >> uv.scale.x >> uv.scale.y >> uv.scale.z;
				}
				else {

					if (!materialData) return;
					// 連結してファイルバスにする
					materialData->textureName_ = directoryPath + token;
					materialData->texHandle_ = TextureManager::Load(directoryPath + token);
					break;
				}
			}
			// uvTransformの値を代入する
			if (materialData && materialData->mapData_) {
				materialData->mapData_->uvTransform = uv.Affine();
			}
		}
	}
#pragma endregion
	//for (auto &material : materialMap_) {
	//	//material.second->CreateBuffer();
	//}
	return;
}


void Model::StartDraw(ID3D12GraphicsCommandList *const commandList) {
	assert(!commandList_ && "EndDrawが呼び出されていません");
	commandList_ = commandList;

	// RootSignatureを設定。
	commandList_->SetGraphicsRootSignature(rootSignature_.Get());
	commandList_->SetPipelineState(graphicsPipelineState_[0].Get());		// PSOを設定


	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い。
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Model::EndDraw() {
	commandList_ = nullptr;
}

Model *const Model::LoadObjFile(const std::string &directoryPath, const std::string &fileName) {

#pragma region 1. ファイルを開く

	std::ifstream file{ defaultDirectory + directoryPath + fileName };
	if (!file.is_open()) return nullptr;		// 開けなかった場合、処理を終了する

#pragma endregion

	Model *const result = new Model;

	result->name_ = fileName.substr(0, fileName.size() - 4);

#pragma region 2. 中で必要になる変数の宣言

	//meshList_.emplace_back(new Mesh);

	Mesh *modelData = nullptr;				// 構築するModelData

	result->meshList_.emplace_back(new Mesh);

	modelData = result->meshList_.back().get();


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
					if (index == "") {	// もし値が無かった場合
						elementIndices[element] = 0;	// 0 を代入(エラー回避)
						continue;
					}
					elementIndices[element] = std::stoi(index);
				}
				// 要素へのIndexから、実際の要素の値を取得して、頂点を機築する
				Vector4 position = positionList[elementIndices[0] - 1];
				Vector3 normal = normalList[elementIndices[2] - 1];
				Vector2 texCoord = ZeroVector2;
				if (elementIndices[1] != 0) {	// 値が無かった場合
					texCoord = texCoordList[elementIndices[1] - 1];
				}
				// 末尾から順に(法線の逆転)
				triangle[2u - faceVertex] = Mesh::VertexData{ position,texCoord,normal };
			}
			// イテレータを用いた末尾への直接構築
			modelData->AddVertex(triangle[0]);
			modelData->AddVertex(triangle[1]);
			modelData->AddVertex(triangle[2]);

		}
		else if (identifier == "o") {
			if (!modelData->vertices_.empty())
				result->meshList_.emplace_back(new Mesh);

			modelData = result->meshList_.back().get();		// 構築するModelData
		}
		else if (identifier == "usemtl") {
			std::string materialID;
			s >> materialID;
			modelData->SetMaterial(result->materialMap_[materialID].get());
		}
		else if (identifier == "mtllib") {

			std::string materialFile;
			s >> materialFile;
			result->LoadMtlFile(directoryPath, materialFile);
			//modelData->material_ = Material::LoadFile(directoryPath, materialFile);
		}
	}
#pragma endregion

	for (auto &mesh : result->meshList_) {
		mesh->CreateBuffer();
		mesh->indexMap_.clear();
	}
	return result;
}

Model *const Model::CreateSphere()
{
	return nullptr;
}

void Model::ImGuiWidget()
{

	for (auto &material : materialMap_) {
		material.second->ImGuiWidget();
	}
}

void Model::Draw(const Transform &transform, const ViewProjection &viewProjection) const
{
	commandList_->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kViewProjection, viewProjection.constBuffer_->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kWorldTransform, transform.constBuffer_->GetGPUVirtualAddress());
	for (auto &mesh : meshList_) {
		mesh->Draw(commandList_);
	}

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

void Mesh::AddVertex(const VertexData &vertex) {
	size_t hashValue = std::hash<VertexData>()(vertex);
	auto it = indexMap_.find(hashValue);
	if (it != indexMap_.end()) {
		// 同じ値のデータが存在する場合
		indexs_.push_back(it->second);
	}
	else {
		// 同じ値のデータが存在しない場合
		const uint32_t index = static_cast<uint32_t>(vertices_.size());
		vertices_.push_back(vertex);
		indexs_.push_back(index);
		indexMap_[hashValue] = index;
	}
}

void Mesh::SetMaterial(Material *const material) {
	if (!material) return;
	material_ = material;
}

void Mesh::Draw(ID3D12GraphicsCommandList *const commandList) const {
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable((uint32_t)Model::RootParameter::kTexture, material_->texHandle_);
	commandList->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kMaterial, material_->constBuffer_->GetGPUVirtualAddress());

	commandList->IASetVertexBuffers(0, 1, &vbView_);
	commandList->IASetIndexBuffer(&ibView_);
	commandList->DrawIndexedInstanced(static_cast<uint32_t>(indexs_.size()), 1, 0, 0, 0);
}

void Material::CreateBuffer() {
	constBuffer_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(MaterialData));
	// マテリアルにデータを書き込む
	// 書き込むためのアドレスを取得
	constBuffer_->Map(0, nullptr, reinterpret_cast<void **>(&mapData_));
	mapData_->color = Vector4{ 1.f,1.f,1.f,1.f };
	mapData_->emissive = Vector4{ 0.f,0.f,0.f,0.f };
	mapData_->uvTransform = Matrix4x4::Identity();
}

void Material::ImGuiWidget()
{
	if (ImGui::TreeNode(name_.c_str())) {
		Transform transform;
		transform.Create(mapData_->uvTransform);

		if (transform.ImGuiWidget2D()) {
			mapData_->uvTransform = transform.Affine();
		}
		if (ImGui::Button("ResetTransform")) {
			mapData_->uvTransform = Matrix4x4::Identity();
		}

		ImGui::ColorEdit4("BaseColor", &mapData_->color.x);
		ImGui::ColorEdit4("EmissiveColor", &mapData_->emissive.x);

		ImGui::TreePop();
	}
}

void Material::Create() {

}


void MinecraftModel::Cube::ResetTransform() {
	enum class vertexPos {
		left = 0b0000,
		right = 0b0001,
		front = 0b0000,
		back = 0b0010,
		up = 0b0000,
		down = 0b0100
	};


}

void MinecraftModel::Cube::Init() {
	CreateBuffer();



}

void MinecraftModel::Cube::CreateBuffer() {
	auto *const device = DirectXCommon::GetInstance()->GetDevice();

	const uint32_t vertexCount = 24u;
	const uint32_t indexCount = 36u;

	vertexBuff = CreateBufferResource(device, sizeof(Mesh::VertexData) * vertexCount);
	indexBuff = CreateBufferResource(device, sizeof(uint32_t) * indexCount);


	// 頂点バッファビューを作成する
	// リソースの先頭のアドレスから使う
	vbView.BufferLocation = vertexBuff->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点の総数のサイズ
	vbView.SizeInBytes = sizeof(Mesh::VertexData) * vertexCount;
	// 1頂点あたりのサイズ
	vbView.StrideInBytes = sizeof(Mesh::VertexData);


	// インデックスview
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.SizeInBytes = sizeof(uint32_t) * indexCount;
	ibView.Format = DXGI_FORMAT_R32_UINT;

}
void MinecraftModel::Bone::UpdateTransform() {
	transform.UpdateMatrix();
	for (auto &child : children) {
		child.UpdateTransform();
	}
}

void MinecraftModel::LoadJson(const std::string &file_path)
{


}
