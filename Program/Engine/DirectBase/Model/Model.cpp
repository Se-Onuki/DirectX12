#include "Model.h"
#include "../../../Utils/Math/Math.hpp"
#include "../../DirectBase/Base/DirectXCommon.h"
#include "../Create/Create.h"
#include <array>

#include "../../../Utils/Math/Transform.h"
#include "../../DirectBase/Base/Shader.h"
#include "../../DirectBase/Base/TextureManager.h"
#include "../../DirectBase/Render/Camera.h"

#include "BoneModel.h"
#include <fstream>
#include <json.hpp>
#include <windows.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

ID3D12GraphicsCommandList *Model::commandList_ = nullptr;

std::array<std::array<Microsoft::WRL::ComPtr<ID3D12PipelineState>, 8u>, 3u> Model::graphicsPipelineState_ = { nullptr };
// std::array<Microsoft::WRL::ComPtr<ID3D12RootSignature>, 2u> Model::rootSignature_ = { nullptr };
// std::array<std::array<PipelineState, 8u>, 2u> Model::graphicsPipelineStateClass_ = {};
std::array<RootSignature, 2u> Model::rootSignatureClass_ = {};
Model::PipelineType Model::sPipelineType_ = Model::PipelineType::kModel;

std::unique_ptr<CBuffer<Matrix4x4>> ModelNode::kIdentity_ = nullptr;

namespace ModelAnimation {

	Animation Animation::CreateFromFile(const std::string &directoryPath, const std::string &filename)
	{
		Animation result{};

		Assimp::Importer importer;

		std::string &&filePath = Model::DefaultDirectory::c_str() + directoryPath + filename;

		const aiScene *scene = importer.ReadFile(filePath.c_str(), 0);

		assert(scene->mNumAnimations != 0 and "アニメーションがありません｡");

		aiAnimation *animationAssimp = scene->mAnimations[0]; // 一旦最初のアニメーションだけ採用する｡ そのうち複数対応するように｡

		// 時間の単位を秒単位に変更
		result.duration_ = static_cast<float>(animationAssimp->mDuration / animationAssimp->mTicksPerSecond);
		// mTicksPerSecond	: 周波数｡ 単位はHz｡
		// mDuration		: mTicksPerSecondで指定された周波数における長さ

		// assimpでは個々のAnimationをchannelと呼んでいるので､channelを回してNodeAnimationの情報を取ってくる｡
		{
			for (uint32_t channelIndex = 0u; channelIndex < animationAssimp->mNumChannels; channelIndex++) {
				// アニメーションのデータのポインタ
				aiNodeAnim *nodeAnimationAssimp = animationAssimp->mChannels[channelIndex];
				// アニメーションの名前から､紐づいた保存先を作成
				NodeAnimation &nodeAnimation = result.nodeAnimations_[nodeAnimationAssimp->mNodeName.C_Str()];

				// 座標データを取得していく
				for (uint32_t keyIndex = 0u; keyIndex < nodeAnimationAssimp->mNumPositionKeys; keyIndex++) {
					// assimp側の座標データ
					aiVectorKey &keyAssimp = nodeAnimationAssimp->mPositionKeys[keyIndex];

					// キーフレームの保存先
					KeyFlameVector3 keyFlame;
					keyFlame.time_ = static_cast<float>(keyAssimp.mTime / animationAssimp->mTicksPerSecond); // 周波数から秒単位に変換
					keyFlame.value_ = Vector3{ -keyAssimp.mValue.x, keyAssimp.mValue.y, keyAssimp.mValue.z };  // 右手から左手に変更

					// データを転送
					nodeAnimation.translate_.keyFlames_.push_back(std::move(keyFlame));
				}

				// 姿勢データを取得していく
				for (uint32_t keyIndex = 0u; keyIndex < nodeAnimationAssimp->mNumRotationKeys; keyIndex++) {
					// assimp側の姿勢データ
					aiQuatKey &keyAssimp = nodeAnimationAssimp->mRotationKeys[keyIndex];

					// キーフレームの保存先
					KeyFlameQuaternion keyFlame;
					keyFlame.time_ = static_cast<float>(keyAssimp.mTime / animationAssimp->mTicksPerSecond);                        // 周波数から秒単位に変換
					keyFlame.value_ = Quaternion{ keyAssimp.mValue.x, -keyAssimp.mValue.y, -keyAssimp.mValue.z, keyAssimp.mValue.w }; // 右手から左手に変更

					// データを転送
					nodeAnimation.rotate_.keyFlames_.push_back(std::move(keyFlame));
				}

				// スケールのデータを取得していく
				for (uint32_t keyIndex = 0u; keyIndex < nodeAnimationAssimp->mNumScalingKeys; keyIndex++) {
					// assimp側の姿勢データ
					aiVectorKey &keyAssimp = nodeAnimationAssimp->mScalingKeys[keyIndex];

					// キーフレームの保存先
					KeyFlameVector3 keyFlame;
					keyFlame.time_ = static_cast<float>(keyAssimp.mTime / animationAssimp->mTicksPerSecond); // 周波数から秒単位に変換
					keyFlame.value_ = Vector3{ keyAssimp.mValue.x, keyAssimp.mValue.y, keyAssimp.mValue.z };   // そのまま代入する

					// データを転送
					nodeAnimation.scale_.keyFlames_.push_back(std::move(keyFlame));
				}
			}
		}

		return result;
	}

	void AnimationPlayer::Start(bool isLoop)
	{
		// アニメーションの初期化
		animationTimer_.Start(animation_->duration_, isLoop); // 時間を設定する
	}

	void AnimationPlayer::Update(float deltaTime, Model *model)
	{
		// Animationの更新
		animationTimer_.Update(deltaTime);

		CalcTransform(animationTimer_.GetNowFlame(), model->rootNode_);

	}

	void AnimationPlayer::CalcTransform(float animateTime, ModelNode &modelNode)
	{
		// 対応したアニメーションがあれば通す
		if (animation_->nodeAnimations_.contains(modelNode.name_)) {
			// モデルのノードに紐づいたアニメーションを取得する
			const auto &rootNodeAnimation = animation_->nodeAnimations_.at(modelNode.name_);
			// ノードのアニメーションのデータを取得する
			BoneModel::SimpleTransform rootTransform;
			if (rootNodeAnimation.scale_.size()) {
				rootTransform.scale_ = rootNodeAnimation.scale_.CalcValue(animateTime);
			}
			if (rootNodeAnimation.rotate_.size()) {
				rootTransform.rotate_ = rootNodeAnimation.rotate_.CalcValue(animateTime);
			}
			if (rootNodeAnimation.translate_.size()) {
				rootTransform.translate_ = rootNodeAnimation.translate_.CalcValue(animateTime);
			}
			// モデルデータの転送
			(*modelNode.localMatrix_) = rootTransform.CalcTransMat();
		}
		for (auto &child : modelNode.children_) {
			CalcTransform(animateTime, child);
		}
	}

}

void Model::StaticInit()
{
	CreatePipeLine();

	ModelNode::kIdentity_ = std::make_unique<CBuffer<Matrix4x4>>(Matrix4x4::Identity());
}

void Model::CreatePipeLine()
{

	// HRESULT hr = S_FALSE;
	// ID3D12Device *const device = DirectXCommon::GetInstance()->GetDevice();

#pragma region PSO(Pipeline State Object)

#pragma region RootParameter

	// RootParameter作成
	std::array<D3D12_ROOT_PARAMETER, static_cast<uint32_t>(RootParameter::kSize)> rootParameters = {};

#pragma region kWorldTransform

	// DescriptorRangeの設定
	D3D12_DESCRIPTOR_RANGE worldDescriptorRange[1] = {};
	worldDescriptorRange[0].BaseShaderRegister = 0;                                                   // 0から始める
	worldDescriptorRange[0].NumDescriptors = 1;                                                       // 数は1つ
	worldDescriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;                              // SRVを使う
	worldDescriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // Offsetを自動計算

	rootParameters[(uint32_t)Model::RootParameter::kWorldTransform].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;           // DescriptorTableを使う
	rootParameters[(uint32_t)Model::RootParameter::kWorldTransform].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;                    // VertexShaderで使う
	rootParameters[(uint32_t)Model::RootParameter::kWorldTransform].DescriptorTable.pDescriptorRanges = worldDescriptorRange;             // Tableの中身の配列を指定
	rootParameters[(uint32_t)Model::RootParameter::kWorldTransform].DescriptorTable.NumDescriptorRanges = _countof(worldDescriptorRange); // Tableで使用する数

#pragma endregion

	rootParameters[(uint32_t)Model::RootParameter::kViewProjection].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;     // CBVを使う
	rootParameters[(uint32_t)Model::RootParameter::kViewProjection].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; // VertexShaderで使う
	rootParameters[(uint32_t)Model::RootParameter::kViewProjection].Descriptor.ShaderRegister = 1;                     // レジスタ番号1とバインド

	rootParameters[(uint32_t)Model::RootParameter::kInstanceLocation].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;     // CBVを使う
	rootParameters[(uint32_t)Model::RootParameter::kInstanceLocation].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; // VertexShaderで使う
	rootParameters[(uint32_t)Model::RootParameter::kInstanceLocation].Descriptor.ShaderRegister = 2;                     // レジスタ番号2とバインド

	rootParameters[(uint32_t)Model::RootParameter::kMaterial].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;    // CBVを使う
	rootParameters[(uint32_t)Model::RootParameter::kMaterial].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	rootParameters[(uint32_t)Model::RootParameter::kMaterial].Descriptor.ShaderRegister = 0;                    // レジスタ番号0とバインド (b0が設定されているので0)

	rootParameters[(uint32_t)Model::RootParameter::kLight].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;    // CBVを使う
	rootParameters[(uint32_t)Model::RootParameter::kLight].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	rootParameters[(uint32_t)Model::RootParameter::kLight].Descriptor.ShaderRegister = 1;                    // レジスタ番号1とバインド

	rootParameters[(uint32_t)Model::RootParameter::kModelTransform].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;  // CBVを使う
	rootParameters[(uint32_t)Model::RootParameter::kModelTransform].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; // PixelShaderで使う
	rootParameters[(uint32_t)Model::RootParameter::kModelTransform].Descriptor.ShaderRegister = 4;                  // レジスタ番号4とバインド

#pragma region Texture

	// DescriptorRangeの設定
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;                                                   // 0から始める
	descriptorRange[0].NumDescriptors = 1;                                                       // 数は1つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;                              // SRVを使う
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // Offsetを自動計算

	rootParameters[(uint32_t)Model::RootParameter::kTexture].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;      // DescriptorTableを使う
	rootParameters[(uint32_t)Model::RootParameter::kTexture].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;                // PixelShaderで使う
	rootParameters[(uint32_t)Model::RootParameter::kTexture].DescriptorTable.pDescriptorRanges = descriptorRange;             // Tableの中身の配列を指定
	rootParameters[(uint32_t)Model::RootParameter::kTexture].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange); // Tableで使用する数

#pragma endregion

	rootSignatureClass_[static_cast<uint32_t>(PipelineType::kParticle)].Init(rootParameters.data(), rootParameters.size());

	rootParameters[(uint32_t)Model::RootParameter::kWorldTransform].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;     // CBVを使う
	rootParameters[(uint32_t)Model::RootParameter::kWorldTransform].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; // VertexShaderで使う
	rootParameters[(uint32_t)Model::RootParameter::kWorldTransform].Descriptor.ShaderRegister = 0;                     // レジスタ番号0とバインド (b0が設定されているので0)

	rootParameters[(uint32_t)Model::RootParameter::kViewProjection].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;  // CBVを使う
	rootParameters[(uint32_t)Model::RootParameter::kViewProjection].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; // VertexShaderで使う
	rootParameters[(uint32_t)Model::RootParameter::kViewProjection].Descriptor.ShaderRegister = 3;                  // レジスタ番号3とバインド

	rootSignatureClass_[static_cast<uint32_t>(PipelineType::kModel)].Init(rootParameters.data(), rootParameters.size());

#pragma endregion

#pragma region InputLayout(インプットレイアウト)

	// InputLayout
	const std::array<D3D12_INPUT_ELEMENT_DESC, 3u> inputElementDescs{
		D3D12_INPUT_ELEMENT_DESC{
			.SemanticName = "POSITION",
			.SemanticIndex = 0,
			.Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
			.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT,
		},
		D3D12_INPUT_ELEMENT_DESC{
			.SemanticName = "TEXCOORD",
			.SemanticIndex = 0,
			.Format = DXGI_FORMAT_R32G32_FLOAT,
			.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT,
		},
		D3D12_INPUT_ELEMENT_DESC{
			.SemanticName = "NORMAL",
			.SemanticIndex = 0,
			.Format = DXGI_FORMAT_R32G32B32_FLOAT,
			.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT,
		} };
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs.data();
	inputLayoutDesc.NumElements = static_cast<UINT>(inputElementDescs.size());

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

	PipelineState::ShaderSet particleShader;
	particleShader.vertex = Shader::Compile(L"Particle.VS.hlsl", L"vs_6_0");
	particleShader.pixel = Shader::Compile(L"Particle.PS.hlsl", L"ps_6_0");

#pragma endregion

#pragma endregion

#pragma region DepthStencilState

#pragma region DepthStencileStateの設定

	// DepthStencileStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を有効にする
	depthStencilDesc.DepthEnable = true;
	// 書き込みします。
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	// 比較関数はLessEqual。つまり、近ければ表示される。
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

#pragma endregion

#pragma endregion

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignatureClass_[static_cast<uint32_t>(PipelineType::kParticle)].Get(); // RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;                                                              // InputLayout
	graphicsPipelineStateDesc.VS = particleShader.vertex->GetBytecode();                                                  // VertexShader
	graphicsPipelineStateDesc.PS = particleShader.pixel->GetBytecode();                                                   // PixelShader
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;                                                           // RasterizeState

	// DSVのFormatを設定する
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;

#pragma region PSOを生成する

	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	// 利用するトポロジ(形状)のタイプ。三角形。
	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// どのように画面に色を打ち込むかの設定(気にしなくても良い)
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

#pragma endregion

	// graphicsPipelineStateClass_[static_cast<uint32_t>(PipelineType::kParticle)][static_cast<uint32_t>(BlendMode::kNone)].SetInputElementDescs(inputElementDescs);
	// graphicsPipelineStateClass_[static_cast<uint32_t>(PipelineType::kParticle)][static_cast<uint32_t>(BlendMode::kNone)].SetShader(particleShader);
	// graphicsPipelineStateClass_[static_cast<uint32_t>(PipelineType::kParticle)][static_cast<uint32_t>(BlendMode::kNone)].Create(rootSignatureClass_[static_cast<uint32_t>(PipelineType::kParticle)], depthStencilDesc);
	BuildPipeLine(PipelineType::kParticle, graphicsPipelineStateDesc);

	auto shadowPipeline = graphicsPipelineStateDesc;

	PipelineState::ShaderSet shadowParticleShader;
	shadowParticleShader.vertex = Shader::Compile(L"Particle.VS.hlsl", L"vs_6_0");
	shadowParticleShader.pixel = Shader::Compile(L"ShaderParticle.PS.hlsl", L"ps_6_0");

	shadowPipeline.VS = shadowParticleShader.vertex->GetBytecode();
	shadowPipeline.PS = shadowParticleShader.pixel->GetBytecode();

	BuildPipeLine(PipelineType::kShadowParticle, shadowPipeline);

	auto modelPipeline = graphicsPipelineStateDesc;

	PipelineState::ShaderSet modelShader;
	modelShader.vertex = Shader::Compile(L"Object3d.VS.hlsl", L"vs_6_0");
	modelShader.pixel = Shader::Compile(L"Object3d.PS.hlsl", L"ps_6_0");

	modelPipeline.VS = modelShader.vertex->GetBytecode();
	modelPipeline.PS = modelShader.pixel->GetBytecode();

	modelPipeline.pRootSignature = rootSignatureClass_[static_cast<uint32_t>(PipelineType::kModel)].Get(); // RootSignature
	modelPipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;

	BuildPipeLine(PipelineType::kModel, modelPipeline);
#pragma endregion
}

void Model::BuildPipeLine(PipelineType type, D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc)
{

	auto *const device = DirectXCommon::GetInstance()->GetDevice();
	HRESULT hr = S_FALSE;

	// BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};

	// 全ての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	graphicsPipelineStateDesc.BlendState = blendDesc;

	// 書き込みをしない
	graphicsPipelineStateDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;

	// 実際に生成
	hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState_[static_cast<uint32_t>(type)][static_cast<uint32_t>(BlendMode::kNone)]));
	assert(SUCCEEDED(hr));

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

	graphicsPipelineStateDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;

	// 実際に生成
	hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState_[static_cast<uint32_t>(type)][static_cast<uint32_t>(BlendMode::kNormal)]));
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
	hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState_[static_cast<uint32_t>(type)][static_cast<uint32_t>(BlendMode::kAdd)]));
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
	hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState_[static_cast<uint32_t>(type)][static_cast<uint32_t>(BlendMode::kSubtract)]));
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
	hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState_[static_cast<uint32_t>(type)][static_cast<uint32_t>(BlendMode::kMultily)]));
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
	hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState_[static_cast<uint32_t>(type)][static_cast<uint32_t>(BlendMode::kScreen)]));
	assert(SUCCEEDED(hr));

#pragma endregion

#pragma region 奥ならば表示

	graphicsPipelineStateDesc.BlendState = D3D12_BLEND_DESC{};
	graphicsPipelineStateDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	// 書き込みをしない
	graphicsPipelineStateDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	graphicsPipelineStateDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_GREATER;

	// 実際に生成
	hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState_[static_cast<uint32_t>(type)][static_cast<uint32_t>(BlendMode::kBacker)]));
	assert(SUCCEEDED(hr));

#pragma endregion

#pragma region 常に表示

	graphicsPipelineStateDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;

	// 実際に生成
	hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState_[static_cast<uint32_t>(type)][static_cast<uint32_t>(BlendMode::kAlways)]));
	assert(SUCCEEDED(hr));

#pragma endregion
}

void Model::LoadMtlFile(const std::string &directoryPath, const std::string &fileName)
{

	Material *materialData = nullptr;
	std::string line;

	std::ifstream file{ Model::DefaultDirectory::c_str() + directoryPath + fileName };
	if (!file.is_open())
		return; // 開けなかった場合、処理を終了する

#pragma region ファイルからMaterialDataを構築

	while (std::getline(file, line)) {

		std::string identifier;
		std::istringstream s{ line };
		s >> identifier;

		// 新規マテリアルの生成
		if (identifier == "newmtl") {
			std::string materialName;
			s >> materialName;
			materialMap_[materialName] = std::make_unique<Material>(materialName);
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
			BoneModel::SimpleTransform uv{};
			uv.scale_ = Vector3::one;

			while (s >> token) {
				if (token == "-o") {
					s >> uv.translate_.x >> uv.translate_.y >> uv.translate_.z;
				}
				else if (token == "-s") {
					s >> uv.scale_.x >> uv.scale_.y >> uv.scale_.z;
				}
				else {

					if (!materialData) {
						return;
					}
					// 連結してファイルバスにする
					materialData->textureName_ = directoryPath + token;
					materialData->texHandle_ = TextureManager::Load(directoryPath + token);
					break;
				}
			}
			// uvTransformの値を代入する
			if (materialData && materialData->materialBuff_) {
				materialData->materialBuff_->uvTransform = uv.CalcTransMat();
			}
		}
	}
#pragma endregion
	// for (auto &material : materialMap_) {
	//	//material.second->CreateBuffer();
	// }
	return;
}

void Model::StartDraw(ID3D12GraphicsCommandList *const commandList)
{
	assert(!commandList_ && "EndDrawが呼び出されていません");
	commandList_ = commandList;

	// RootSignatureを設定。
	commandList_->SetGraphicsRootSignature(rootSignatureClass_[static_cast<uint32_t>(PipelineType::kModel)].Get());
	commandList_->SetPipelineState(graphicsPipelineState_[static_cast<uint32_t>(PipelineType::kModel)][0].Get()); // PSOを設定

	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い。
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	sPipelineType_ = Model::PipelineType::kModel;
}

void Model::EndDraw()
{
	commandList_ = nullptr;
}

std::unique_ptr<Model> Model::LoadObjFile(const std::string &directoryPath, const std::string &fileName)
{

#pragma region 1. ファイルを開く

	std::ifstream file{ DefaultDirectory::c_str() + directoryPath + fileName };
	if (!file.is_open()) {
		return nullptr;
	} // 開けなかった場合、処理を終了する

#pragma endregion

	auto result = std::make_unique<Model>();

	result->name_ = fileName.substr(0, fileName.size() - 4);

#pragma region 2. 中で必要になる変数の宣言

	// meshList_.emplace_back(new Mesh);

	Mesh *modelData = nullptr;        // 構築するModelData
	Material *materialData = nullptr; // マテリアルの共用

	result->meshList_.emplace_back(std::make_unique<Mesh>())->pNode_ = &result->rootNode_;

	modelData = result->meshList_.back().get();

	std::vector<Vector4> positionList; // 位置
	std::vector<Vector3> normalList;   // 法線
	std::vector<Vector2> texCoordList; // テクスチャ座標

	std::string line; // ファイルから一行を格納する

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
					std::getline(v, index, '/');     // 区切りでインデックスを読んでいく
					if (index == "") {               // もし値が無かった場合
						elementIndices[element] = 0; // 0 を代入(エラー回避)
						continue;
					}
					elementIndices[element] = std::stoi(index);
				}
				// 要素へのIndexから、実際の要素の値を取得して、頂点を機築する
				Vector4 position = positionList[elementIndices[0] - 1];
				Vector3 normal = normalList[elementIndices[2] - 1];
				Vector2 texCoord = ZeroVector2;
				if (elementIndices[1] != 0) { // 値が無かった場合
					texCoord = texCoordList[elementIndices[1] - 1];
				}
				// 末尾から順に(法線の逆転)
				triangle[2u - faceVertex] = Mesh::VertexData{ position, texCoord, normal };
			}
			// イテレータを用いた末尾への直接構築
			modelData->AddVertex(triangle[0]);
			modelData->AddVertex(triangle[1]);
			modelData->AddVertex(triangle[2]);
		}
		else if (identifier == "o") {
			if (!modelData->vertices_.empty()) {
				result->meshList_.emplace_back(std::make_unique<Mesh>())->pNode_ = &result->rootNode_;
			}
			modelData = result->meshList_.back().get(); // 構築するModelData

			if (materialData) { // もしマテリアルが設定されてたら割り当て
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
			// modelData->material_ = Material::LoadFile(directoryPath, materialFile);
		}
	}
#pragma endregion

	for (auto &mesh : result->meshList_) {
		mesh->CreateBuffer();
		mesh->indexMap_.clear();
		mesh->vertices_.clear();
		mesh->indexs_.clear();
	}
	return std::move(result);
}

std::unique_ptr<Model> Model::LoadAssimpModelFile(const std::string &directoryPath, const std::string &fileName)
{
	// マテリアルのID
	constexpr uint32_t defaultMaterialIndex = 0u;

	// モデルの生成
	std::unique_ptr<Model> result = std::make_unique<Model>();

	// ファイルの名前
	std::string filePath = DefaultDirectory::c_str() + directoryPath + fileName;

	// assimpのローダ
	Assimp::Importer importer;

	// assimpのscene作成 / 三角形の並び順を逆に / UVの上下を反転
	const aiScene *const scene = importer.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
	// メッシュが存在しない場合はエラーとする
	assert(scene->HasMeshes() and "Meshが存在しない場合はエラーとする");

	// マテリアルの取得
	for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {

		// 保存先のマテリアル
		std::unique_ptr<Material> materialItem = std::make_unique<Material>();
		// 保存先のデータの構築
		materialItem->Create();
		// シーン内のマテリアル
		aiMaterial *material = scene->mMaterials[materialIndex];
		// ディフューズのテクスチャが存在するか
		if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {

			// 文字列の保存先
			aiString textureFilePath;
			// マテリアルからテクスチャ名を取得
			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);

			// テクスチャ名の代入
			materialItem->textureName_ = textureFilePath.C_Str();
			// テクスチャの読み込み
			materialItem->texHandle_ = TextureManager::Load(directoryPath + textureFilePath.C_Str());
		}

		// マテリアル名の設定
		materialItem->name_ = material->GetName().C_Str();

		// 数値の文字列をキーとしてマテリアルのデータを追加
		result->materialMap_.insert({ std::to_string(materialIndex), std::move(materialItem) });
	}

	// メッシュの読み込み
	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		std::unique_ptr<Mesh> meshResult = std::make_unique<Mesh>();

		// メッシュのポインタ
		const aiMesh *const mesh = scene->mMeshes[meshIndex];
		assert(mesh->HasNormals() and "法線が無いメッシュは今回は非対応");
		assert(mesh->HasTextureCoords(defaultMaterialIndex) and "Texcoordの無いMeshは今回は非対応");

		// Faceの解析
		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
			// Faceの参照
			const aiFace &face = mesh->mFaces[faceIndex];

			assert(face.mNumIndices == 3u and "三角形のみサポート");

			// Vertexの解析
			for (uint32_t element = 0; element < face.mNumIndices; ++element) {
				// 頂点ID
				uint32_t vertexIndex = face.mIndices[element];
				// 3次元座標
				const aiVector3D &position = mesh->mVertices[vertexIndex];
				// 法線
				const aiVector3D &normal = mesh->mNormals[vertexIndex];
				// テクスチャ座標
				const aiVector3D &texcoord = mesh->mTextureCoords[defaultMaterialIndex][vertexIndex];
				// 頂点データ
				Mesh::VertexData vertex;
				vertex.position = { position.x, position.y, position.z, 1.f };
				vertex.normal = { normal.x, normal.y, normal.z };
				vertex.texCoord = { texcoord.x, texcoord.y };

				// データの補正
				// aiProcess_MakeLeftHandedは z *= -1 で、右手->左手に変換するので手動で対処
				vertex.position.x *= -1.f;
				vertex.normal.x *= -1.f;

				// 頂点の追加
				meshResult->AddVertex(vertex);
			}
		}
		// マテリアルのポインタを取得
		meshResult->material_ = result->materialMap_.at(SoLib::to_string(mesh->mMaterialIndex)).get();
		// 保存されたデータからメッシュを作成
		meshResult->CreateBuffer();

		// 名前の設定
		meshResult->meshName_ = mesh->mName.C_Str();

		// メッシュをコンテナに保存
		result->meshList_.push_back(std::move(meshResult));
	}

	// ノードの構成
	result->rootNode_ = ModelNode::Create(scene->mRootNode);

	for (auto &mesh : result->meshList_) {
		const ModelNode *const node = result->rootNode_.FindNode(mesh->meshName_);
		//assert(node and "名前に一致するノードがありません");
		if (node) {
			mesh->pNode_ = node;
		}
		else {
			mesh->pNode_ = &result->rootNode_;
		}
	}

	result->name_ = fileName;

	return std::move(result);
}

std::unique_ptr<Model> Model::CreateSphere()
{
	return nullptr;
}

bool Model::ImGuiWidget()
{
	bool result = false;
	if (ImGui::TreeNode(name_.c_str())) {
		for (auto &material : materialMap_) {
			result |= material.second->ImGuiWidget();
		}
		ImGui::TreePop();
	}
	return result;
}

void Model::SetPipelineType(const PipelineType pipelineType)
{
	// 設定されてるシグネチャとが一致していない場合
	if (sPipelineType_ != pipelineType) {
		if (pipelineType != PipelineType::kShadowParticle) {

			commandList_->SetGraphicsRootSignature(rootSignatureClass_[static_cast<uint32_t>(pipelineType)].Get());
			commandList_->SetPipelineState(graphicsPipelineState_[static_cast<uint32_t>(pipelineType)][0].Get());
			// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い。
			commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		}
		else {
			commandList_->SetGraphicsRootSignature(rootSignatureClass_[static_cast<uint32_t>(PipelineType::kParticle)].Get());
			commandList_->SetPipelineState(graphicsPipelineState_[static_cast<uint32_t>(pipelineType)][0].Get());
			// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い。
			commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		}
		sPipelineType_ = pipelineType;
	}
}

std::unique_ptr<Model> Model::CreatePlane()
{
	auto newModel = std::make_unique<Model>();

	newModel->meshList_.push_back(std::make_unique<Mesh>());
	auto &mesh = *newModel->meshList_.begin();

	auto &vertexArray = mesh->vertexBuffer_;
	vertexArray.SetVertexData(std::array{ Mesh::VertexData{}, Mesh::VertexData{}, Mesh::VertexData{}, Mesh::VertexData{} });
	vertexArray.SetIndexData(std::array{ 0u, 1u, 2u, 1u, 3u, 2u });

	// 左下
	vertexArray.GetVertexData()[0u].position = { -0.5f, -0.5f, 0.f, 1.f };
	vertexArray.GetVertexData()[0u].texCoord = { 0.f, 1.f };
	vertexArray.GetVertexData()[0u].normal = { 0.f, 0.f, -1.f };
	// 左上
	vertexArray.GetVertexData()[1u].position = { -0.5f, +0.5f, 0.f, 1.f };
	vertexArray.GetVertexData()[1u].texCoord = { 0.f, 0.f };
	vertexArray.GetVertexData()[1u].normal = { 0.f, 0.f, -1.f };

	// 右下
	vertexArray.GetVertexData()[2u].position = { +0.5f, -0.5f, 0.f, 1.f };
	vertexArray.GetVertexData()[2u].texCoord = { 1.f, 1.f };
	vertexArray.GetVertexData()[2u].normal = { 0.f, 0.f, -1.f };
	// 右上
	vertexArray.GetVertexData()[3u].position = { +0.5f, +0.5f, 0.f, 1.f };
	vertexArray.GetVertexData()[3u].texCoord = { 1.f, 0.f };
	vertexArray.GetVertexData()[3u].normal = { 0.f, 0.f, -1.f };

	newModel->materialMap_["default"] = (std::make_unique<Material>());
	auto &material = newModel->materialMap_["default"];

	newModel->name_ = "plane";

	material->Create();
	material->blendMode_ = Model::BlendMode::kNone;

	mesh->SetMaterial(material.get());
	mesh->pNode_ = &newModel->rootNode_;

	return std::move(newModel);
}

void Model::Draw(const Transform &transform, const Camera3D &camera) const
{
	assert(sPipelineType_ == PipelineType::kModel && "設定されたシグネチャがkModelではありません");

	commandList_->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kViewProjection, camera.constData_.GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kWorldTransform, transform.GetGPUVirtualAddress());
	for (auto &mesh : meshList_) {
		commandList_->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kModelTransform, mesh->pNode_->GetLocalMatrix().GetGPUVirtualAddress());
		commandList_->SetPipelineState(graphicsPipelineState_[static_cast<uint32_t>(PipelineType::kModel)][static_cast<uint32_t>(mesh->GetMaterial()->blendMode_)].Get()); // PSOを設定
		mesh->Draw(commandList_);
	}
}

void Model::Draw(const Transform &transform, const Camera3D &camera, const Material &material) const
{
	assert(sPipelineType_ == PipelineType::kModel && "設定されたシグネチャがkModelではありません");

	commandList_->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kViewProjection, camera.constData_.GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kWorldTransform, transform.GetGPUVirtualAddress());

	for (auto &mesh : meshList_) {
		commandList_->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kModelTransform, mesh->pNode_->GetLocalMatrix().GetGPUVirtualAddress());
		commandList_->SetPipelineState(graphicsPipelineState_[static_cast<uint32_t>(PipelineType::kModel)][static_cast<uint32_t>(material.blendMode_)].Get()); // PSOを設定
		mesh->Draw(commandList_, 1u, &material);
	}
}

void Model::Draw(const D3D12_GPU_DESCRIPTOR_HANDLE &transformSRV, uint32_t drawCount, const CBuffer<uint32_t> &drawIndex, const Camera3D &camera) const
{
	assert((sPipelineType_ == PipelineType::kParticle || sPipelineType_ == PipelineType::kShadowParticle) && "設定されたシグネチャがkParticleではありません");

	commandList_->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kViewProjection, camera.constData_.GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kInstanceLocation, drawIndex.GetGPUVirtualAddress());
	commandList_->SetGraphicsRootDescriptorTable((uint32_t)Model::RootParameter::kWorldTransform, transformSRV);

	for (auto &mesh : meshList_) {
		commandList_->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kModelTransform, mesh->pNode_->GetLocalMatrix().GetGPUVirtualAddress());
		commandList_->SetPipelineState(graphicsPipelineState_[static_cast<uint32_t>(sPipelineType_)][static_cast<uint32_t>(mesh->GetMaterial()->blendMode_)].Get()); // PSOを設定
		mesh->Draw(commandList_, drawCount);
	}
}

void Model::Draw(const D3D12_GPU_DESCRIPTOR_HANDLE &transformSRV, uint32_t drawCount, const CBuffer<uint32_t> &drawIndex, const CBuffer<Camera3D::CameraMatrix> &camera) const
{
	assert((sPipelineType_ == PipelineType::kParticle || sPipelineType_ == PipelineType::kShadowParticle) && "設定されたシグネチャがkParticleではありません");

	commandList_->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kViewProjection, camera.GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kInstanceLocation, drawIndex.GetGPUVirtualAddress());
	commandList_->SetGraphicsRootDescriptorTable((uint32_t)Model::RootParameter::kWorldTransform, transformSRV);

	for (auto &mesh : meshList_) {
		commandList_->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kModelTransform, mesh->pNode_->GetLocalMatrix().GetGPUVirtualAddress());
		commandList_->SetPipelineState(graphicsPipelineState_[static_cast<uint32_t>(sPipelineType_)][static_cast<uint32_t>(mesh->GetMaterial()->blendMode_)].Get()); // PSOを設定
		mesh->Draw(commandList_, drawCount);
	}
}

void Mesh::CreateBuffer()
{

#pragma region 頂点バッファ
	// 頂点バッファへのデータ転送
	vertexBuffer_.SetVertexData(vertices_);

#pragma endregion

#pragma region インデックスバッファ

	// indexバッファへのデータ転送
	vertexBuffer_.SetIndexData(indexs_);

#pragma endregion
}

void Mesh::AddVertex(const VertexData &vertex)
{
	auto it = indexMap_.find(vertex);
	if (it != indexMap_.end()) {
		// 同じ値のデータが存在する場合
		indexs_.push_back(it->second);
	}
	else {
		// 同じ値のデータが存在しない場合
		const uint32_t index = static_cast<uint32_t>(vertices_.size());
		vertices_.push_back(vertex);
		indexs_.push_back(index);
		indexMap_[vertex] = index;
	}
}

void Mesh::SetMaterial(Material *const material)
{
	if (!material)
		return;
	material_ = material;
}

void Mesh::Draw(ID3D12GraphicsCommandList *const commandList, uint32_t drawCount, const Material *const material) const
{
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable((uint32_t)Model::RootParameter::kTexture, material_->texHandle_);
	if (material) {
		commandList->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kMaterial, material->materialBuff_.GetGPUVirtualAddress());
	}
	else {
		commandList->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kMaterial, material_->materialBuff_.GetGPUVirtualAddress());
	}
	commandList->IASetVertexBuffers(0, 1, &vertexBuffer_.GetVBView());
	commandList->IASetIndexBuffer(&vertexBuffer_.GetIBView());
	commandList->DrawIndexedInstanced(static_cast<uint32_t>(vertexBuffer_.GetIndexData().size()), drawCount, 0, 0, 0);
}

void Material::CreateBuffer()
{
	// マテリアルにデータを書き込む
	this->Create();
	/*materialBuff_->emissive = Vector4{ 0.f,0.f,0.f,0.f };
	materialBuff_->uvTransform = Matrix4x4::Identity();*/
}

bool Material::ImGuiWidget()
{
	bool result = false;
	if (ImGui::TreeNode(name_.c_str())) {
		static BaseTransform transform;
		transform.MatToSRT(materialBuff_->uvTransform);

		if (transform.ImGuiWidget2D()) {
			materialBuff_->uvTransform = transform.Affine();
		}
		if (ImGui::Button("ResetTransform")) {
			materialBuff_->uvTransform = Matrix4x4::Identity();
		}

		ImGui::ColorEdit4("BaseColor", &materialBuff_->color.r);
		ImGui::ColorEdit3("EmissiveColor", &materialBuff_->emissive.x);

		const static std::array<std::string, 6u> blendStr{ "kNone", "kNormal", "kAdd", "kSubtract", "kMultily", "kScreen" };

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
		result |= ImGui::SliderFloat("Shininess", &materialBuff_->shininess, 0.f, 1.f);

		ImGui::TreePop();
	}
	return result;
}

void Material::Create()
{

	texHandle_ = TextureManager::LoadDefaultTexture();
	name_ = "default";

	blendMode_ = Model::BlendMode::kNone;

	materialBuff_ = Material::MaterialData{
		.color = Vector4{1.f, 1.f, 1.f, 1.f},
		.emissive = {},
		.uvTransform = Matrix4x4::Identity(),
		.shininess = 1.f,
	};
}

void MinecraftModel::Cube::ResetTransform()
{
	enum vertexPos {
		left = 0b0000,
		right = 0b0001,
		front = 0b0000,
		back = 0b0010,
		up = 0b0000,
		down = 0b0100
	};
}

MinecraftModel::Cube::Cube(const Cube &other)
{
	this->faces_ = other.faces_;
}

void MinecraftModel::Cube::Init()
{
	for (auto &face : faces_) {
		face.Init();
	}
}

void MinecraftModel::Face::Draw(ID3D12GraphicsCommandList *const commandList)
{
	commandList->IASetVertexBuffers(0, 1, &vbView_);
	commandList->IASetIndexBuffer(&ibView_);
	commandList->DrawIndexedInstanced(6u, 1, 0, 0, 0);
}
void MinecraftModel::Cube::Draw(ID3D12GraphicsCommandList *const commandList)
{
	for (auto &face : faces_) {
		face.Draw(commandList);
	}
}

void MinecraftModel::Cube::SetVertex(const Vector3 &origin, const Vector3 &size)
{
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
		centor + Vector3{-size.x, size.y, size.z},  // 左遠
		centor + Vector3{size.x, size.y, size.z},   // 右遠
		centor + Vector3{-size.x, size.y, -size.z}, // 左近
		centor + Vector3{size.x, size.y, -size.z},  // 右近

		centor + Vector3{-size.x, -size.y, size.z},  // 左遠
		centor + Vector3{size.x, -size.y, size.z},   // 右遠
		centor + Vector3{-size.x, -size.y, -size.z}, // 左近,
		centor + Vector3{size.x, -size.y, -size.z},  // 右近
	};

	faces_[(uint32_t)FaceDirection::UP].SetVertex(
		std::array<Vector3, 4u>{
		vertices[left + front + up],
			vertices[right + front + up],
			vertices[left + back + up],
			vertices[right + back + up],
	},
		+Vector3::up);

	faces_[(uint32_t)FaceDirection::Down].SetVertex(
		{
			vertices[left + back + down],
			vertices[right + back + down],
			vertices[left + front + down],
			vertices[right + front + down],
		},
		-Vector3::up);

	faces_[(uint32_t)FaceDirection::FRONT].SetVertex(
		{
			vertices[right + front + up],
			vertices[left + front + up],
			vertices[right + front + down],
			vertices[left + front + down],
		},
		+Vector3::front);

	faces_[(uint32_t)FaceDirection::BACK].SetVertex(
		{
			vertices[left + back + up],
			vertices[right + back + up],
			vertices[left + back + down],
			vertices[right + back + down],
		},
		-Vector3::front);

	faces_[(uint32_t)FaceDirection::RIGHT].SetVertex(
		{
			vertices[right + back + up],
			vertices[right + front + up],
			vertices[right + back + down],
			vertices[right + front + down],
		},
		+Vector3::right);

	faces_[(uint32_t)FaceDirection::LEFT].SetVertex(
		{
			vertices[left + front + up],
			vertices[left + back + up],
			vertices[left + front + down],
			vertices[left + back + down],
		},
		-Vector3::right);
}

void MinecraftModel::Bone::Draw(ID3D12GraphicsCommandList *const commandList)
{
	for (auto &cube : cubes_) {
		cube.Draw(commandList);
	}
	for (auto &child : children_) {
		child.second.Draw(commandList);
	}
}
void MinecraftModel::Bone::UpdateTransform()
{
	transform_->UpdateMatrix();
	for (auto &child : children_) {
		child.second.UpdateTransform();
	}
}

void MinecraftModel::Bone::SetParent(Bone *const parent)
{
	parent_ = parent;
	transform_->parent_ = parent->transform_->parent_;
}

void MinecraftModel::Draw(ID3D12GraphicsCommandList *const commandList)
{
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
			bone.transform_->parent_ = &transformOrigin_;
		}

		// 座標構築
		// 原点
		const auto &pivotJson = boneJson["pivot"];
		Vector3 pivot = Vector3{
			(float)pivotJson.at(0).get<double>(),
			(float)pivotJson.at(1).get<double>(),
			(float)pivotJson.at(2).get<double>() };
		bone.transform_->translate = pivot / 16.f;

		// 回転
		const auto &rotateJson = boneJson.find("rotation");
		if (rotateJson != boneJson.end()) {
			Vector3 rotate = Vector3{
				(float)rotateJson->at(0).get<double>(),
				(float)rotateJson->at(1).get<double>(),
				(float)rotateJson->at(2).get<double>() };
			bone.transform_->rotate = SoLib::MakeQuaternion(rotate * Angle::Dig2Rad);
		}
		else {
			bone.transform_->rotate = Quaternion::Identity;
		}

		// キューブ
		const auto &cubesJson = boneJson.find("cubes");
		if (cubesJson != boneJson.end()) {
			for (const auto &cubeText : *cubesJson) {
				// Cubeの構築
				bone.cubes_.emplace_back();
				Cube &cube = bone.cubes_.back();
				cube.Init();

				Vector3 origin = cubeText.at("origin");
				Vector3 size = cubeText.at("size");
				origin /= 16.f;
				size /= 16.f;

				cube.SetVertex(origin, size);
			}
		}
	}
}

MinecraftModel::Face::Face(const Face &other)
{
	CreateBuffer();
	*vertices_ = *(other.vertices_);
	*indexs_ = *(other.indexs_);
}

void MinecraftModel::Face::CreateBuffer()
{
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

void MinecraftModel::Face::Init()
{
	CreateBuffer();
	// 左上からZの形に構成
	indexs_[0] = 0u;
	indexs_[1] = 1u;
	indexs_[2] = 2u;
	indexs_[3] = 1u;
	indexs_[4] = 3u;
	indexs_[5] = 2u;
}

void MinecraftModel::Face::SetVertex(const std::array<Vector3, 4u> &vertex, const Vector3 &face)
{
	for (uint8_t i = 0u; i < 4u; i++) {
		vertices_[i].position = Vector4{ vertex[i].x, vertex[i].y, vertex[i].z, 1.f };
		vertices_[i].normal = face;
		vertices_[i].texCoord = { 0.f, 0.f };
	}
}

ModelNode ModelNode::Create(aiNode *node)
{
	// 返す値
	ModelNode result{};

	aiVector3D scale, translate;
	aiQuaternion rotate;

	// transformを取得
	node->mTransformation.Decompose(scale, rotate, translate);

	result.transform_.scale_ = { scale.x, scale.y, scale.z };
	result.transform_.rotate_ = { rotate.x, -rotate.y, -rotate.z, rotate.w };
	result.transform_.translate_ = { -translate.x, translate.y, translate.z };


	// 行列を代入
	result.localMatrix_ = std::make_unique<CBuffer<Matrix4x4>>(result.transform_.Affine());

	// 名前の設定
	result.name_ = node->mName.C_Str();

	// 子ノードの数だけ領域を確保
	result.children_.resize(node->mNumChildren);

	// 子ノードの設定
	for (uint32_t i = 0; i < node->mNumChildren; i++) {
		// 再起的に読み込む
		result.children_[i] = ModelNode::Create(node->mChildren[i]);
	}

	return result;
}

const CBuffer<Matrix4x4> &ModelNode::GetLocalMatrix() const
{
	if (localMatrix_) {
		return *localMatrix_;
	}
	else {
		return *ModelNode::kIdentity_;
	}
}

void ModelNode::CalcAffine()
{
	if (localMatrix_) {
		*localMatrix_ = transform_.Affine();
	}
}

const ModelNode *ModelNode::FindNode(const std::string &name) const
{
	// 名前が一致していたらそれを返す
	if (name_ == name) { return this; }
	for (const auto &child : children_) {
		const auto *ptr = child.FindNode(name);
		// 名前を検索して、一致したら返す
		if (ptr) { return ptr; }
	}
	// 一致しなかったらnullptrを返す
	return nullptr;
}

Skeleton Skeleton::MakeSkeleton(const ModelNode &rootNode)
{
	Skeleton result;
	result.root_ = ModelJoint::MakeJointIndex(rootNode, {}, result.joints_);

	// 名前とindexを紐づける
	for (const auto &joint : result.joints_) {
		result.jointMap_.emplace(joint->name_, joint->index_);
	}

	return result;
}

void Skeleton::UpdateMatrix()
{
	for (auto &joint : joints_) {
		joint->CalcAffine();
		if (joint->parent_) {
			joint->skeletonSpaceMatrix_ = joint->localMatrix_ * joints_[*joint->parent_]->skeletonSpaceMatrix_;
		}
		else {
			joint->skeletonSpaceMatrix_ = joint->localMatrix_;
		}
	}

}

void Skeleton::ApplyAnimation(const ModelAnimation::Animation &animation, const float animateTime)
{
	for (auto &joint : joints_) {


	}
}

uint32_t ModelJoint::MakeJointIndex(const ModelNode &node, const std::optional<uint32_t> parent, std::vector<std::unique_ptr<ModelJoint>> &joints)
{
	std::unique_ptr<ModelJoint> joint = std::make_unique<ModelJoint>();
	joint->name_ = node.name_;
	joint->localMatrix_ = *node.localMatrix_;
	joint->skeletonSpaceMatrix_ = Matrix4x4::Identity();
	joint->transform_ = node.transform_;
	uint32_t selfIndex = joint->index_ = static_cast<uint32_t>(joints.size());
	joint->parent_ = parent;
	joints.push_back(std::move(joint));

	for (const ModelNode &child : node.children_) {

		uint32_t childIndex = ModelJoint::MakeJointIndex(child, selfIndex, joints);
		joints[selfIndex]->children_.push_back(childIndex);
	}

	return selfIndex;
}
