#include "Model.h"
#include "../Create/Create.h"
#include "../../../Utils/Math/Math.hpp"
#include <array>
#include "../../DirectBase/Base/DirectXCommon.h"

#include "../../DirectBase/Render/Camera.h"
#include "../../../Utils/Math/Transform.h"
#include "../../DirectBase/Base/TextureManager.h"
#include "../../DirectBase/Base/Shader.h"

#include <windows.h>
#include <fstream>
#include <json.hpp>

ID3D12GraphicsCommandList *Model::commandList_ = nullptr;
const char *const Model::defaultDirectory = "resources/";

std::array<Microsoft::WRL::ComPtr<ID3D12PipelineState>, 6u> Model::graphicsPipelineState_ = { nullptr };
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
	graphicsPipelineStateDesc.pRootSignature = rootSignature_.Get();	// RootSignature
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
	hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState_[static_cast<uint32_t>(BlendMode::kNone)]));
	assert(SUCCEEDED(hr));

#pragma endregion


#pragma region BlendState(ブレンドステート) ノーマルブレンド

	// NormalBlend
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	graphicsPipelineStateDesc.BlendState = blendDesc;

	// 実際に生成
	hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState_[static_cast<uint32_t>(BlendMode::kNormal)]));
	assert(SUCCEEDED(hr));


#pragma endregion

#pragma region BlendState(ブレンドステート) 加算合成

	// NormalBlend
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;

	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	graphicsPipelineStateDesc.BlendState = blendDesc;

	// 実際に生成
	hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState_[static_cast<uint32_t>(BlendMode::kAdd)]));
	assert(SUCCEEDED(hr));


#pragma endregion

#pragma region BlendState(ブレンドステート) 減算合成

	// NormalBlend
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;

	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	graphicsPipelineStateDesc.BlendState = blendDesc;

	// 実際に生成
	hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState_[static_cast<uint32_t>(BlendMode::kSubtract)]));
	assert(SUCCEEDED(hr));


#pragma endregion

#pragma region BlendState(ブレンドステート) 乗算合成

	// NormalBlend
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;

	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	graphicsPipelineStateDesc.BlendState = blendDesc;

	// 実際に生成
	hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState_[static_cast<uint32_t>(BlendMode::kMultily)]));
	assert(SUCCEEDED(hr));


#pragma endregion

#pragma region BlendState(ブレンドステート) スクリーン合成

	// NormalBlend
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;

	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	graphicsPipelineStateDesc.BlendState = blendDesc;

	// 実際に生成
	hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState_[static_cast<uint32_t>(BlendMode::kScreen)]));
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
			// 仮読み込み
			materialData->textureName_ = "white2x2.png";
			materialData->texHandle_ = TextureManager::Load("white2x2.png");
		}
		else if (identifier == "Kd") {
			if (materialData && materialData->materialBuff_) {
				Vector4 &color = materialData->materialBuff_->color;
				s >> color.x >> color.y >> color.z;
			}
		}

		else if (identifier == "map_Kd") {

			std::string token;
			Transform uv{};
			uv.scale = Vector3::one;

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
			if (materialData && materialData->materialBuff_) {
				materialData->materialBuff_->uvTransform = uv.Affine();
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
	Material *materialData = nullptr;		// マテリアルの共用

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

			if (materialData) {								// もしマテリアルが設定されてたら割り当て
				modelData->SetMaterial(materialData);
			}
		}
		else if (identifier == "usemtl") {
			std::string materialID;
			s >> materialID;
			materialData = result->materialMap_[materialID].get();
			modelData->SetMaterial(materialData);
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
		mesh->vertices_.clear();
		mesh->indexs_.clear();
	}
	return result;
}

Model *const Model::CreateSphere()
{
	return nullptr;
}

void Model::ImGuiWidget()
{
	if (ImGui::TreeNode(name_.c_str())) {
		for (auto &material : materialMap_) {
			material.second->ImGuiWidget();
		}
		ImGui::TreePop();
	}
}

void Model::Draw(const Transform &transform, const Camera<Render::CameraType::Projecction> &camera) const
{
	commandList_->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kViewProjection, camera.constData_.GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kWorldTransform, transform.mapBuffer_.GetGPUVirtualAddress());
	for (auto &mesh : meshList_) {
		commandList_->SetPipelineState(graphicsPipelineState_[static_cast<uint32_t>(mesh->GetMaterial()->blendMode_)].Get());		// PSOを設定
		mesh->Draw(commandList_);
	}

}

void Mesh::CreateBuffer() {

#pragma region 頂点バッファ
	// 頂点バッファへのデータ転送
	vertexBuffer_.SetVertexData(vertices_);

#pragma endregion

#pragma region インデックスバッファ

	// indexバッファへのデータ転送
	vertexBuffer_.SetIndexData(indexs_);

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
	commandList->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kMaterial, material_->materialBuff_.GetGPUVirtualAddress());

	commandList->IASetVertexBuffers(0, 1, &vertexBuffer_.GetVBView());
	commandList->IASetIndexBuffer(&vertexBuffer_.GetIBView());
	commandList->DrawIndexedInstanced(static_cast<uint32_t>(vertexBuffer_.GetIndexData().size()), 1, 0, 0, 0);
}

void Material::CreateBuffer() {
	// マテリアルにデータを書き込む
	materialBuff_->color = Vector4{ 1.f,1.f,1.f,1.f };
	materialBuff_->emissive = Vector4{ 0.f,0.f,0.f,0.f };
	materialBuff_->uvTransform = Matrix4x4::Identity();
}

void Material::ImGuiWidget()
{
	if (ImGui::TreeNode(name_.c_str())) {
		Transform transform;
		transform.Create(materialBuff_->uvTransform);

		if (transform.ImGuiWidget2D()) {
			materialBuff_->uvTransform = transform.Affine();
		}
		if (ImGui::Button("ResetTransform")) {
			materialBuff_->uvTransform = Matrix4x4::Identity();
		}

		ImGui::ColorEdit4("BaseColor", &materialBuff_->color.x);
		ImGui::ColorEdit3("EmissiveColor", &materialBuff_->emissive.x);

		const static std::array<std::string, 6u>blendStr{ "kNone", "kNormal","kAdd", "kSubtract", "kMultily", "kScreen" };

		if (ImGui::BeginCombo("BlendMode", blendStr[static_cast<uint32_t>(blendMode_)].c_str())) {

			for (uint32_t i = 0; i < blendStr.size(); i++) {
				if (ImGui::Selectable(blendStr[i].c_str())) {
					blendMode_ = static_cast<Model::BlendMode>(i);
					break;
				}
			}
			ImGui::EndCombo();
		}

		if (ImGui::TreeNode("Texture")) {
			texHandle_ = TextureManager::GetInstance()->ImGuiTextureSelecter(texHandle_);
			ImGui::TreePop();
		}

		ImGui::TreePop();
	}
}

void Material::Create() {

}


void MinecraftModel::Cube::ResetTransform() {
	enum vertexPos {
		left = 0b0000,
		right = 0b0001,
		front = 0b0000,
		back = 0b0010,
		up = 0b0000,
		down = 0b0100
	};


}

MinecraftModel::Cube::Cube(const Cube &other) {
	this->faces_ = other.faces_;
}

void MinecraftModel::Cube::Init() {
	for (auto &face : faces_) {
		face.Init();
	}
}

void MinecraftModel::Face::Draw(ID3D12GraphicsCommandList *const commandList) {
	commandList->IASetVertexBuffers(0, 1, &vbView_);
	commandList->IASetIndexBuffer(&ibView_);
	commandList->DrawIndexedInstanced(6u, 1, 0, 0, 0);
}
void MinecraftModel::Cube::Draw(ID3D12GraphicsCommandList *const commandList) {
	for (auto &face : faces_) {
		face.Draw(commandList);
	}
}

void MinecraftModel::Cube::SetVertex(const Vector3 &origin, const Vector3 &size) {
	enum vertexPos {
		left = 0b0000,
		right = 0b0001,
		front = 0b0000,
		back = 0b0010,
		up = 0b0000,
		down = 0b0100
	};

	const Vector3 centor = origin + size / 2.f;
	std::array<Vector3, 8u> vertices{
		centor + Vector3{-size.x,size.y,size.z},	// 左遠
		centor + Vector3{size.x,size.y,size.z},		// 右遠
		centor + Vector3{-size.x,size.y,-size.z},	// 左近
		centor + Vector3{size.x,size.y,-size.z},	// 右近

		centor + Vector3{-size.x,-size.y,size.z},	// 左遠
		centor + Vector3{size.x,-size.y,size.z}, 	// 右遠
		centor + Vector3{-size.x,-size.y,-size.z},	// 左近,
		centor + Vector3{size.x,-size.y,-size.z},	// 右近
	};

	faces_[(uint32_t)FaceDirection::UP].SetVertex(
		std::array<Vector3, 4u>{
		vertices[left + front + up],
			vertices[right + front + up],
			vertices[left + back + up],
			vertices[right + back + up],
	},
		+Vector3::up
	);

	faces_[(uint32_t)FaceDirection::Down].SetVertex(
		{
		vertices[left + back + down],
		vertices[right + back + down],
		vertices[left + front + down],
		vertices[right + front + down],
		},
		-Vector3::up
		);

	faces_[(uint32_t)FaceDirection::FRONT].SetVertex(
		{
		vertices[right + front + up],
		vertices[left + front + up],
		vertices[right + front + down],
		vertices[left + front + down],
		},
		+Vector3::front
		);

	faces_[(uint32_t)FaceDirection::BACK].SetVertex(
		{
		vertices[left + back + up],
		vertices[right + back + up],
		vertices[left + back + down],
		vertices[right + back + down],
		},
		-Vector3::front
		);

	faces_[(uint32_t)FaceDirection::RIGHT].SetVertex(
		{
		vertices[right + back + up],
		vertices[right + front + up],
		vertices[right + back + down],
		vertices[right + front + down],
		},
		+Vector3::right
		);

	faces_[(uint32_t)FaceDirection::LEFT].SetVertex(
		{
		vertices[left + front + up],
		vertices[left + back + up],
		vertices[left + front + down],
		vertices[left + back + down],
		},
		-Vector3::right
		);
}

void MinecraftModel::Bone::Draw(ID3D12GraphicsCommandList *const commandList) {
	for (auto &cube : cubes_) {
		cube.Draw(commandList);
	}
	for (auto &child : children_) {
		child.second.Draw(commandList);
	}
}
void MinecraftModel::Bone::UpdateTransform() {
	transform_.UpdateMatrix();
	for (auto &child : children_) {
		child.second.UpdateTransform();
	}
}

void MinecraftModel::Bone::SetParent(Bone *const parent)
{
	parent_ = parent;
	transform_.parent_ = parent->transform_.parent_;
}

void MinecraftModel::Draw(ID3D12GraphicsCommandList *const commandList) {
	for (auto &bone : bones_) {
		bone.second.Draw(commandList);
	}
}

void MinecraftModel::LoadJson(const std::string &file_path)
{
	std::ifstream ifs;
	ifs.open(file_path);
	// ファイルオープンエラー
	if (ifs.fail()) {
		std::string message = "Failed open data file for read.";
		MessageBoxA(nullptr, message.c_str(), "GlobalVariables", 0);
		assert(0);
		return;
	}

	nlohmann::json root;
	ifs >> root;
	ifs.close();


	const auto &geometry = root["minecraft:geometry"][0];

#pragma region 単ピクセルサイズ

	texelSize_.x = 1.f / geometry["description"]["texture_width"].get<int32_t>();
	texelSize_.y = 1.f / geometry["description"]["texture_height"].get<int32_t>();

#pragma endregion

	const auto &bones = geometry["bones"];
	for (const auto &boneJson : bones) {
		// ボーン名
		const std::string &boneName = boneJson["name"];
		// ボーンの構築、名前設定
		Bone &bone = bones_[boneName];
		bone.name_ = boneName;

		// 親子関係
		const auto &parent = boneJson.find("parent");
		if (parent != boneJson.end()) {
			const std::string &parentName = parent->get<std::string>();
			bone.SetParent(&bones_[parentName]);
		}
		else {
			bone.transform_.parent_ = &transformOrigin_;
		}

		// 座標構築
		// 原点
		const auto &pivotJson = boneJson["pivot"];
		Vector3 pivot = Vector3{
			(float)pivotJson.at(0).get<double>(),
			(float)pivotJson.at(1).get<double>(),
			(float)pivotJson.at(2).get<double>()
		};
		bone.transform_.translate = pivot / 16.f;

		// 回転
		const auto &rotateJson = boneJson.find("rotation");
		if (rotateJson != boneJson.end()) {
			Vector3 rotate = Vector3{
				(float)rotateJson->at(0).get<double>(),
				(float)rotateJson->at(1).get<double>(),
				(float)rotateJson->at(2).get<double>()
			};
			bone.transform_.rotate = rotate * Angle::Dig2Rad;
		}
		else {
			bone.transform_.rotate = Vector3::zero;
		}

		// キューブ
		const auto &cubesJson = boneJson.find("cubes");
		if (cubesJson != boneJson.end()) {
			for (const auto &cubeText : *cubesJson) {
				// Cubeの構築
				bone.cubes_.emplace_back();
				Cube &cube = bone.cubes_.back();
				cube.Init();

				const auto &originJson = cubeText.at("origin");
				Vector3 origin = Vector3{
					(float)originJson.at(0).get<double>(),
					(float)originJson.at(1).get<double>(),
					(float)originJson.at(2).get<double>()
				};
				const auto &sizeJson = cubeText.at("size");
				Vector3 size = Vector3{
					(float)sizeJson.at(0).get<double>(),
					(float)sizeJson.at(1).get<double>(),
					(float)sizeJson.at(2).get<double>()
				};
				origin /= 16.f;
				size /= 16.f;

				cube.SetVertex(origin, size);
			}
		}
	}

}

MinecraftModel::Face::Face(const Face &other) {
	CreateBuffer();
	*vertices_ = *(other.vertices_);
	*indexs_ = *(other.indexs_);
}

void MinecraftModel::Face::CreateBuffer() {
	auto *const device = DirectXCommon::GetInstance()->GetDevice();

	const uint32_t vertexCount = 4u;
	const uint32_t indexCount = 6u;

	vertexBuff_ = CreateBufferResource(device, sizeof(Mesh::VertexData) * vertexCount);
	indexBuff_ = CreateBufferResource(device, sizeof(uint32_t) * indexCount);

	vertexBuff_->Map(0, nullptr, reinterpret_cast<void **>(&vertices_));
	indexBuff_->Map(0, nullptr, reinterpret_cast<void **>(&indexs_));

	// 頂点バッファビューを作成する
	// リソースの先頭のアドレスから使う
	vbView_.BufferLocation = vertexBuff_->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点の総数のサイズ
	vbView_.SizeInBytes = sizeof(Mesh::VertexData) * vertexCount;
	// 1頂点あたりのサイズ
	vbView_.StrideInBytes = sizeof(Mesh::VertexData);


	// インデックスview
	ibView_.BufferLocation = indexBuff_->GetGPUVirtualAddress();
	ibView_.SizeInBytes = sizeof(uint32_t) * indexCount;
	ibView_.Format = DXGI_FORMAT_R32_UINT;
}

void MinecraftModel::Face::Init() {
	CreateBuffer();
	// 左上からZの形に構成
	indexs_[0] = 0u; indexs_[1] = 1u; indexs_[2] = 2u;
	indexs_[3] = 1u; indexs_[4] = 3u; indexs_[5] = 2u;
}

void MinecraftModel::Face::SetVertex(const std::array<Vector3, 4u> &vertex, const Vector3 &face)
{
	for (uint8_t i = 0u; i < 4u; i++) {
		vertices_[i].position = Vector4{ vertex[i].x,vertex[i].y,vertex[i].z,1.f };
		vertices_[i].normal = face;
		vertices_[i].texCoord = { 0.f,0.f };
	}
}
