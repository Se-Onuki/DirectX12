/// @file Model.cpp
/// @brief モデルの実装(旧モデル)
/// @author ONUKI seiya
#include "Model.h"
#include "../../Engine/Utils/Math/Math.hpp"
#include "../../DirectBase/Base/DirectXCommon.h"
#include "../Create/Create.h"
#include <array>

#include "../../Engine/Utils/Math/Transform.h"
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
#include "../../Engine/Utils/Convert/Convert.h"
#include "../Render/CameraManager.h"
#include "../../ResourceObject/ResourceObjectManager.h"
#include <execution>
#include "SkeletonReference.h"
#include "SkeletonAnimation/Skeleton.h"
#include "../Render/SkyBox/SkyBox.h"
#include "SkeletonAnimation/ModelAnimation.h"

ID3D12GraphicsCommandList *Model::commandList_ = nullptr;

std::array<std::array<Microsoft::WRL::ComPtr<ID3D12PipelineState>, 8u>, static_cast<uint32_t>(Model::PipelineType::kSize)> Model::graphicsPipelineState_ = { nullptr };
// std::array<Microsoft::WRL::ComPtr<ID3D12RootSignature>, 2u> Model::rootSignature_ = { nullptr };
// std::array<std::array<PipelineState, 8u>, 2u> Model::graphicsPipelineStateClass_ = {};
std::array<RootSignature, 2u> Model::rootSignatureClass_ = {};
Model::PipelineType Model::sPipelineType_ = Model::PipelineType::kModel;

//DescHeapCbvSrvUav::HeapRange Model::srvRange_;

std::unique_ptr<CBuffer<Matrix4x4>> ModelNode::kIdentity_ = nullptr;

void Model::StaticInit()
{
	CreatePipeLine();

	ModelNode::kIdentity_ = std::make_unique<CBuffer<Matrix4x4>>(Matrix4x4::Identity());
	//srvRange_ = DirectXCommon::GetInstance()->GetSRVHeap()->RequestHeapAllocation(kModelHeapCount_);
}

void Model::CreatePipeLine()
{

	// HRESULT hr = S_FALSE;
	// ID3D12Device *const device = DirectXCommon::GetInstance()->GetDevice();

#pragma region PSO(Pipeline State Object)

#pragma region RootParameter

	// RootParameter作成
	std::array<D3D12_ROOT_PARAMETER, static_cast<uint32_t>(RootParameter::kSize)> rootParameters = {};

#pragma region kSkyBox

	// DescriptorRangeの設定
	D3D12_DESCRIPTOR_RANGE skyDesc[1] = {};
	skyDesc[0].BaseShaderRegister = 1;                                                   // 1から始める
	skyDesc[0].NumDescriptors = 1;                                                       // 数は1つ
	skyDesc[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;                              // SRVを使う
	skyDesc[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // Offsetを自動計算

	rootParameters[(uint32_t)Model::RootParameter::kSkyBox].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;           // DescriptorTableを使う
	rootParameters[(uint32_t)Model::RootParameter::kSkyBox].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;                    // PixelShaderで使う
	rootParameters[(uint32_t)Model::RootParameter::kSkyBox].DescriptorTable.pDescriptorRanges = skyDesc;             // Tableの中身の配列を指定
	rootParameters[(uint32_t)Model::RootParameter::kSkyBox].DescriptorTable.NumDescriptorRanges = _countof(skyDesc); // Tableで使用する数

#pragma endregion

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
	rootParameters[(uint32_t)Model::RootParameter::kViewProjection].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; // ALLで使う
	rootParameters[(uint32_t)Model::RootParameter::kViewProjection].Descriptor.ShaderRegister = 3;                     // レジスタ番号3とバインド

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

	// DescriptorRangeの設定
	D3D12_DESCRIPTOR_RANGE paletteDescriptorRange[1] = {};
	paletteDescriptorRange[0].BaseShaderRegister = 1;                                                   // 1から始める
	paletteDescriptorRange[0].NumDescriptors = 1;                                                       // 数は1つ
	paletteDescriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;                              // SRVを使う
	paletteDescriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // Offsetを自動計算

	rootParameters[(uint32_t)Model::RootParameter::kMatrixPalette].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;           // DescriptorTableを使う
	rootParameters[(uint32_t)Model::RootParameter::kMatrixPalette].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;                    // VertexShaderで使う
	rootParameters[(uint32_t)Model::RootParameter::kMatrixPalette].DescriptorTable.pDescriptorRanges = paletteDescriptorRange;             // Tableの中身の配列を指定
	rootParameters[(uint32_t)Model::RootParameter::kMatrixPalette].DescriptorTable.NumDescriptorRanges = _countof(paletteDescriptorRange); // Tableで使用する数

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
	rootSignatureClass_[static_cast<uint32_t>(PipelineType::kModel)].Init(rootParameters.data(), rootParameters.size());

#pragma endregion

#pragma region InputLayout(インプットレイアウト)

	// InputLayout
	const std::array<D3D12_INPUT_ELEMENT_DESC, 5u> inputElementDescs{
		D3D12_INPUT_ELEMENT_DESC{
			.SemanticName = "POSITION",
				.SemanticIndex = 0,
				.Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
				.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT,
		},
		D3D12_INPUT_ELEMENT_DESC{
			.SemanticName = "NORMAL",
			.SemanticIndex = 0,
			.Format = DXGI_FORMAT_R32G32B32_FLOAT,
			.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT,
		},
		D3D12_INPUT_ELEMENT_DESC{
			.SemanticName = "TEXCOORD",
			.SemanticIndex = 0,
			.Format = DXGI_FORMAT_R32G32_FLOAT,
			.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT,
		},
		{
			.SemanticName = "WEIGHT",
			.SemanticIndex = 0,
			.Format = DXGI_FORMAT_R32G32B32A32_FLOAT,	// float4
			.InputSlot = 1,								// 1番目のSlotのVBVだと伝える
			.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT,
		},
		{
			.SemanticName = "INDEX",
			.SemanticIndex = 0,
			.Format = DXGI_FORMAT_R32G32B32A32_UINT,	// uint4
			.InputSlot = 1,								// 1番目のSlotのVBVだと伝える
			.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT,
		},
	};
	D3D12_INPUT_LAYOUT_DESC skinInputLayoutDesc{};
	skinInputLayoutDesc.pInputElementDescs = inputElementDescs.data();
	skinInputLayoutDesc.NumElements = static_cast<UINT>(inputElementDescs.size());

	D3D12_INPUT_LAYOUT_DESC defaultInputLayoutDesc{};
	defaultInputLayoutDesc.pInputElementDescs = inputElementDescs.data();
	defaultInputLayoutDesc.NumElements = 3;
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

	auto *pShaderManager_ = SolEngine::ShaderManager::GetInstance();


	SolEngine::PipelineState::ShaderSet particleShader;
	particleShader.vertex_ = pShaderManager_->Load({ L"Particle.VS.hlsl", L"vs_6_0" });
	particleShader.pixel_ = pShaderManager_->Load({ L"Particle.PS.hlsl", L"ps_6_0" });

#pragma endregion

#pragma endregion

#pragma region DepthStencilState

#pragma region DepthStencileStateの設定
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
	graphicsPipelineStateDesc.InputLayout = defaultInputLayoutDesc;                                                       // InputLayout
	graphicsPipelineStateDesc.VS = particleShader.vertex_->GetBytecode();                                                  // VertexShader
	graphicsPipelineStateDesc.PS = particleShader.pixel_->GetBytecode();                                                   // PixelShader
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

	/*graphicsPipelineStateClass_[static_cast<uint32_t>(PipelineType::kParticle)][static_cast<uint32_t>(BlendMode::kNone)].SetInputElementDescs(inputElementDescs);
	graphicsPipelineStateClass_[static_cast<uint32_t>(PipelineType::kParticle)][static_cast<uint32_t>(BlendMode::kNone)].SetShader(particleShader);
	graphicsPipelineStateClass_[static_cast<uint32_t>(PipelineType::kParticle)][static_cast<uint32_t>(BlendMode::kNone)].Create(rootSignatureClass_[static_cast<uint32_t(PipelineType::kParticle)], depthStencilDesc);*/
	BuildPipeLine(PipelineType::kParticle, graphicsPipelineStateDesc);
	{
		auto shadowPipeline = graphicsPipelineStateDesc;

		SolEngine::PipelineState::ShaderSet shadowParticleShader;
		shadowParticleShader.vertex_ = pShaderManager_->Load({ L"Particle.VS.hlsl", L"vs_6_0" });
		shadowParticleShader.pixel_ = pShaderManager_->Load({ L"ShaderParticle.PS.hlsl", L"ps_6_0" });

		shadowPipeline.VS = shadowParticleShader.vertex_->GetBytecode();
		shadowPipeline.PS = shadowParticleShader.pixel_->GetBytecode();

		BuildPipeLine(PipelineType::kShadowParticle, shadowPipeline);
	}

	{
		auto modelPipeline = graphicsPipelineStateDesc;

		SolEngine::PipelineState::ShaderSet modelShader;
		modelShader.vertex_ = pShaderManager_->Load({ L"Object3d.VS.hlsl", L"vs_6_0" });
		modelShader.pixel_ = pShaderManager_->Load({ L"Object3d.PS.hlsl", L"ps_6_0" });

		modelPipeline.VS = modelShader.vertex_->GetBytecode();
		modelPipeline.PS = modelShader.pixel_->GetBytecode();

		modelPipeline.pRootSignature = rootSignatureClass_[static_cast<uint32_t>(PipelineType::kModel)].Get(); // RootSignature
		modelPipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;

		BuildPipeLine(PipelineType::kModel, modelPipeline);
	}

	{
		auto skinParticlePipeline = graphicsPipelineStateDesc;

		SolEngine::PipelineState::ShaderSet shadowParticleShader;
		shadowParticleShader.vertex_ = pShaderManager_->Load({ L"SkinningParticle.VS.hlsl", L"vs_6_0" });
		shadowParticleShader.pixel_ = pShaderManager_->Load({ L"ShaderParticle.PS.hlsl", L"ps_6_0" });

		skinParticlePipeline.InputLayout = skinInputLayoutDesc;

		skinParticlePipeline.VS = shadowParticleShader.vertex_->GetBytecode();
		skinParticlePipeline.PS = shadowParticleShader.pixel_->GetBytecode();

		BuildPipeLine(PipelineType::kSkinParticle, skinParticlePipeline);
	}

	{
		auto skinModelPipeline = graphicsPipelineStateDesc;

		SolEngine::PipelineState::ShaderSet modelShader;
		modelShader.vertex_ = pShaderManager_->Load({ L"SkinningObject3d.VS.hlsl", L"vs_6_0" });
		modelShader.pixel_ = pShaderManager_->Load({ L"Object3d.PS.hlsl", L"ps_6_0" });

		skinModelPipeline.InputLayout = skinInputLayoutDesc;

		skinModelPipeline.VS = modelShader.vertex_->GetBytecode();
		skinModelPipeline.PS = modelShader.pixel_->GetBytecode();

		skinModelPipeline.pRootSignature = rootSignatureClass_[static_cast<uint32_t>(PipelineType::kModel)].Get(); // RootSignature
		skinModelPipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;

		BuildPipeLine(PipelineType::kSkinModel, skinModelPipeline);
	}



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

void Model::SetSkyBox(const SolEngine::SkyBox &skyBox)
{
	commandList_->SetGraphicsRootDescriptorTable(static_cast<uint32_t>(RootParameter::kSkyBox), TextureManager::GetInstance()->GetGpuSrvHandle(skyBox.GetTextureHandle()));
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

	Mesh *modelData = nullptr;        // 構築するModelData
	Material *materialData = nullptr; // マテリアルの共用

	result->meshList_.emplace_back(std::make_unique<Mesh>())->pNode_ = result->rootNode_.get();

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
				Vector2 texCoord = SoLib::ZeroVector2;
				if (elementIndices[1] != 0) { // 値が無かった場合
					texCoord = texCoordList[elementIndices[1] - 1];
				}
				// 末尾から順に(法線の逆転)
				triangle[2u - faceVertex] = Mesh::VertexData{ position, normal, texCoord };
			}
			// イテレータを用いた末尾への直接構築
			modelData->AddVertex(triangle[0]);
			modelData->AddVertex(triangle[1]);
			modelData->AddVertex(triangle[2]);
		}
		else if (identifier == "o") {
			if (!modelData->vertices_.empty()) {
				result->meshList_.emplace_back(std::make_unique<Mesh>())->pNode_ = result->rootNode_.get();
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
		//assert(mesh->HasTextureCoords(defaultMaterialIndex) and "Texcoordの無いMeshは今回は非対応");

		meshResult->vertexBuffer_.Resize(mesh->mNumVertices);

		// 頂点として解析する
		for (uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; vertexIndex++) {
			// 3次元座標
			const aiVector3D &position = mesh->mVertices[vertexIndex];
			// 法線
			const aiVector3D &normal = mesh->mNormals[vertexIndex];
			// 頂点データ
			Mesh::VertexData &vertex = meshResult->vertexBuffer_.GetVertexData()[vertexIndex];
			vertex.position = { position.x, position.y, position.z, 1.f };
			vertex.normal = { normal.x, normal.y, normal.z };

			// テクスチャ座標
			const aiVector3D *texcoord = mesh->mTextureCoords[defaultMaterialIndex];

			if (texcoord) { vertex.texCoord = { texcoord[vertexIndex].x, texcoord[vertexIndex].y }; }

			// データの補正
			// aiProcess_MakeLeftHandedは z *= -1 で、右手->左手に変換するので手動で対処
			vertex.position.x *= -1.f;
			vertex.normal.x *= -1.f;
		}
		// Faceの解析
		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
			// Faceの参照
			const aiFace &face = mesh->mFaces[faceIndex];

			assert(face.mNumIndices == 3u and "三角形のみサポート");

			std::span<uint32_t> elements{ face.mIndices, face.mNumIndices };
			for (uint32_t index : elements) {
				meshResult->indexs_.push_back(index);
			}
		}

		// マテリアルのポインタを取得
		meshResult->material_ = result->materialMap_.at(SoLib::to_string(mesh->mMaterialIndex)).get();

		// SkinCluster構築用のデータ取得を追加
		for (uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
			aiBone *aiBone = mesh->mBones[boneIndex];
			std::string jointName = aiBone->mName.C_Str();
			// 名前をもとにデータを構築
			JointWeightData &jointWeightData = result->skinCluster_.skinClusterData_[jointName];

			// バインド行列の逆行列を取得
			aiMatrix4x4 bindPoseMatrixAssimp = aiBone->mOffsetMatrix.Inverse();
			Matrix4x4 bindPoseMatrix = SoLib::Convert<Matrix4x4>(bindPoseMatrixAssimp);
			// 逆行列を保存
			jointWeightData.inverseBindPoseMatrix_ = bindPoseMatrix.InverseSRT();

			// 領域を確保しておく
			jointWeightData.vertexWeightData_.resize(aiBone->mNumWeights);
			for (uint32_t weightIndex = 0; weightIndex < aiBone->mNumWeights; ++weightIndex) {
				aiVertexWeight aiWeight = aiBone->mWeights[weightIndex];
				// ウェイトデータを取得して格納
				jointWeightData.vertexWeightData_[weightIndex] = { .weight_ = aiWeight.mWeight, .vertexIndex_ = aiWeight.mVertexId };
			}


		}

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
		const ModelNode *const node = result->rootNode_->FindNode(mesh->meshName_);
		//assert(node and "名前に一致するノードがありません");
		if (node) {
			mesh->pNode_ = node;
		}
		else {
			mesh->pNode_ = result->rootNode_.get();
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
	if (pipelineType == PipelineType::kModel or pipelineType == PipelineType::kSkinModel) {
		commandList_->SetGraphicsRootSignature(rootSignatureClass_[static_cast<uint32_t>(PipelineType::kModel)].Get());
	}
	else {
		commandList_->SetGraphicsRootSignature(rootSignatureClass_[static_cast<uint32_t>(PipelineType::kParticle)].Get());
	}
	commandList_->SetPipelineState(graphicsPipelineState_[static_cast<uint32_t>(pipelineType)][0].Get());
	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い。
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	sPipelineType_ = pipelineType;
}

std::unique_ptr<Model> Model::CreatePlane()
{
	auto newModel = std::make_unique<Model>();

	newModel->meshList_.push_back(std::make_unique<Mesh>());
	auto &mesh = *newModel->meshList_.begin();

	auto &vertexArray = mesh->vertexBuffer_;
	auto &indexBuffer = mesh->indexBuffer_;
	vertexArray.SetVertexData(std::array{ Mesh::VertexData{}, Mesh::VertexData{}, Mesh::VertexData{}, Mesh::VertexData{} });
	indexBuffer.SetIndexData(std::array{ 0u, 1u, 2u, 1u, 3u, 2u });

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
	mesh->pNode_ = newModel->rootNode_.get();

	return std::move(newModel);
}

void Model::Draw(const SoLib::Transform &transform, const SolEngine::Camera3D &camera) const
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


void Model::Draw(const SkinCluster &skinCluster, const SoLib::Transform &transform, const SolEngine::Camera3D &camera) const
{
	assert(sPipelineType_ == PipelineType::kSkinModel && "設定されたシグネチャがkSkinModelではありません");

	commandList_->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kViewProjection, camera.constData_.GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kWorldTransform, transform.GetGPUVirtualAddress());
	commandList_->SetGraphicsRootDescriptorTable((uint32_t)Model::RootParameter::kMatrixPalette, skinCluster.GetPalette().GetHeapRange().GetHandle(0).gpuHandle_);

	for (uint32_t i = 0; i < meshList_.size(); i++) {
		auto &mesh = meshList_[i];
		// commandList_->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kModelTransform, mesh->pNode_->GetLocalMatrix().GetGPUVirtualAddress());
		commandList_->SetPipelineState(graphicsPipelineState_[static_cast<uint32_t>(PipelineType::kSkinModel)][static_cast<uint32_t>(mesh->GetMaterial()->blendMode_)].Get()); // PSOを設定
		mesh->Draw(commandList_, 1, &skinCluster.skeletonRef_->modelInfluence_->influence_.GetVBView());
	}
}

void Model::Draw(const SkinCluster &skinCluster, const D3D12_GPU_DESCRIPTOR_HANDLE &transformSRV, uint32_t drawCount, const CBuffer<uint32_t> &drawIndex, const SolEngine::Camera3D &camera) const
{
	assert(sPipelineType_ == PipelineType::kSkinParticle && "設定されたシグネチャがkSkinParticleではありません");

	commandList_->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kViewProjection, camera.constData_.GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kInstanceLocation, drawIndex.GetGPUVirtualAddress());
	commandList_->SetGraphicsRootDescriptorTable((uint32_t)Model::RootParameter::kWorldTransform, transformSRV);
	commandList_->SetGraphicsRootDescriptorTable((uint32_t)Model::RootParameter::kMatrixPalette, skinCluster.GetPalette().GetHeapRange().GetHandle(0).gpuHandle_);

	for (uint32_t i = 0; i < meshList_.size(); i++) {
		auto &mesh = meshList_[i];
		commandList_->SetPipelineState(graphicsPipelineState_[static_cast<uint32_t>(PipelineType::kSkinParticle)][static_cast<uint32_t>(mesh->GetMaterial()->blendMode_)].Get()); // PSOを設定
		mesh->Draw(commandList_, drawCount, &skinCluster.skeletonRef_->modelInfluence_->influence_.GetVBView());
	}
}

void Model::Draw(const D3D12_GPU_DESCRIPTOR_HANDLE &transformSRV, uint32_t drawCount, const CBuffer<uint32_t> &drawIndex, const SolEngine::Camera3D &camera) const
{
	assert((sPipelineType_ == PipelineType::kParticle || sPipelineType_ == PipelineType::kShadowParticle) && "設定されたシグネチャがkParticleではありません");

	commandList_->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kViewProjection, camera.constData_.GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kInstanceLocation, drawIndex.GetGPUVirtualAddress());
	commandList_->SetGraphicsRootDescriptorTable((uint32_t)Model::RootParameter::kWorldTransform, transformSRV);

	for (auto &mesh : meshList_) {
		const CBuffer<Matrix4x4> *node = nullptr;
		if (mesh->pNode_) { node = &mesh->pNode_->GetLocalMatrix(); }
		else {
			node = ModelNode::kIdentity_.get();
		}
		commandList_->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kModelTransform, node->GetGPUVirtualAddress());
		commandList_->SetPipelineState(graphicsPipelineState_[static_cast<uint32_t>(sPipelineType_)][static_cast<uint32_t>(mesh->GetMaterial()->blendMode_)].Get()); // PSOを設定
		mesh->Draw(commandList_, drawCount);
	}
}

void Model::Draw(const D3D12_GPU_DESCRIPTOR_HANDLE &transformSRV, uint32_t drawCount, const CBuffer<uint32_t> &drawIndex, const CBuffer<SolEngine::Camera3D::CameraMatrix> &camera) const
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
	indexBuffer_.SetIndexData(indexs_);

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

void Mesh::Draw(ID3D12GraphicsCommandList *const commandList, uint32_t drawCount, const D3D12_VERTEX_BUFFER_VIEW *vbv, uint32_t vertexOffset) const
{

	std::array<D3D12_VERTEX_BUFFER_VIEW, 2u> vbvs = { vertexBuffer_.GetVBView() };
	if (vbv) {
		vbvs[1] = *vbv;
	}

	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable((uint32_t)Model::RootParameter::kTexture, material_->texHandle_);

	commandList->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kMaterial, material_->materialBuff_.GetGPUVirtualAddress());
	commandList->IASetVertexBuffers(0, vbv ? 2 : 1, vbvs.data());
	commandList->IASetIndexBuffer(&indexBuffer_.GetIBView());
	commandList->DrawIndexedInstanced(static_cast<uint32_t>(indexBuffer_.GetIndexData().size()), drawCount, 0, vertexOffset, 0);
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
		static SoLib::BaseTransform transform;
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
		.ambient = {},
		.uvTransform = Matrix4x4::Identity(),
		.shininess = 1.f,
		.shininessStrength = 0.f,
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
		MessageBoxA(nullptr, message.c_str(), "MinecraftModel", 0);
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
			bone.transform_->parent_ = *transformOrigin_;
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
			bone.transform_->rotate = SoLib::MakeQuaternion(rotate * SoLib::Angle::Dig2Rad);
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

std::unique_ptr<ModelNode> ModelNode::Create(aiNode *node)
{
	// 返す値
	std::unique_ptr<ModelNode> result = std::make_unique<ModelNode>();

	aiVector3D scale, translate;
	aiQuaternion rotate;

	result->transform_ = SoLib::Convert<SoLib::SimpleTransformQuaternion>(node->mTransformation);

	// 行列を代入
	result->localMatrix_ = std::make_unique<CBuffer<Matrix4x4>>(result->transform_.Affine());

	// 名前の設定
	result->name_ = node->mName.C_Str();

	// 子ノードの数だけ領域を確保
	result->children_.resize(node->mNumChildren);

	// 子ノードの配列
	const std::span<aiNode *> nodeChildren{ node->mChildren, node->mNumChildren };

	// 子ノードの設定
	// 再帰的に読み込む
	std::transform(nodeChildren.begin(), nodeChildren.end(), result->children_.begin(), [](aiNode *child) {return ModelNode::Create(child); });

	//// 子ノードの設定
	//for (uint32_t i = 0; i < node->mNumChildren; i++) {
	//	// 再起的に読み込む
	//	result->children_[i] = ModelNode::Create(node->mChildren[i]);
	//}

	return std::move(result);
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
		const ModelNode *ptr = child->FindNode(name);
		// 名前を検索して、一致したら返す
		if (ptr) { return ptr; }
	}
	// 一致しなかったらnullptrを返す
	return nullptr;
}

std::unique_ptr<SkeletonState> SkeletonState::MakeSkeleton(const SolEngine::SkeletonAnimation::Skeleton *skeleton)
{
	std::unique_ptr<SkeletonState> result = std::make_unique<SkeletonState>();

	// ジョイント数と同じメモリを確保する
	result->joints_.resize(skeleton->skeletonReference_->joints_.size());

	//std::transform(skeleton->skeletonReference_->joints_.begin(), skeleton->skeletonReference_->joints_.end(), result->joints_.begin(), [](const std::unique_ptr<ModelJointReference> &ref) {
	//	ModelJointState result;
	//	result.localMatrix_ = node->GetLocalMatrix();
	//	result.skeletonSpaceMatrix_ = Matrix4x4::Identity();
	//	result.transform_ = ref->transform_;
	//	return result;
	//	}
	//);

	// ノードからジョイントを構築し、現在のジョイントのindexを保存する
	// ModelJointState::MakeJointIndex(rootNode, result->joints_);

	result->reference_ = *skeleton->skeletonReference_;

	return std::move(result);
}

void SkeletonState::UpdateMatrix()
{
	for (uint32_t i = 0; i < joints_.size(); i++) {
		const auto &ref = reference_->joints_[i];
		auto &joint = joints_[i];
		joint.CalcAffine();
		if (ref->parent_ != (std::numeric_limits<uint32_t>::max)()) {
			joint.skeletonSpaceMatrix_ = joint.localMatrix_ * joints_[ref->parent_].skeletonSpaceMatrix_;
		}
		else {
			joint.skeletonSpaceMatrix_ = joint.localMatrix_;
		}
	}

}

void SkeletonState::ApplyAnimation(const SolEngine::SkeletonAnimation::Animation &animation, const float animateTime)
{

	for (uint32_t i = 0; i < joints_.size(); i++) {
		const auto &ref = reference_->joints_[i];
		auto &joint = joints_[i];
		// 対象のJointのAnimationがあれば、値の適用を行う。下記のif文はC++17から可能になった初期化付きif文。
		if (auto it = animation.nodeAnimations_.find(ref->name_); it != animation.nodeAnimations_.end()) {
			const auto &rootNodeAnimation = it->second;
			// 時間に応じた値を取得して代入する
			joint.transform_.scale_ = rootNodeAnimation.scale_.CalcValue(animateTime);
			joint.transform_.rotate_ = rootNodeAnimation.rotate_.CalcValue(animateTime);
			joint.transform_.translate_ = rootNodeAnimation.translate_.CalcValue(animateTime);

		}

	}
}
uint32_t ModelJointReference::MakeJointIndex(const ModelNode *node, const std::optional<uint32_t> parent, std::vector<std::unique_ptr<ModelJointReference>> &joints)
{
	// 領域の確保回数を減らす
	joints.reserve(joints.size() + 1 + node->children_.size());
	// ジョイントの構築
	auto joint = std::make_unique<ModelJointReference>();
	// 名前の転送
	joint->name_ = node->name_;
	// 現在のジョイント数を計算
	uint32_t selfIndex = joint->index_ = static_cast<uint32_t>(joints.size());
	// 親が存在するなら代入
	if (parent) { joint->parent_ = parent; }
	// 書き込んだデータを追加
	joints.push_back(std::move(joint));
	// 子のデータを書き込む
	for (const auto &child : node->children_) {
		// 子のIndexを返して､子のデータを構築
		uint32_t childIndex = ModelJointReference::MakeJointIndex(child.get(), selfIndex, joints);
		// 自分のデータに子のデータを書き込む
		joints[selfIndex]->children_.push_back(childIndex);
	}
	// 自分の番号を返す
	return selfIndex;
}


uint32_t ModelJointState::MakeJointIndex(const ModelNode *node, std::vector<std::unique_ptr<ModelJointState>> &joints)
{
	// 領域の確保回数を減らす
	joints.reserve(joints.size() + 1 + node->children_.size());

	std::unique_ptr<ModelJointState> joint = std::make_unique<ModelJointState>();
	joint->localMatrix_ = node->GetLocalMatrix();
	joint->skeletonSpaceMatrix_ = Matrix4x4::Identity();
	joint->transform_ = node->transform_;
	const uint32_t selfIndex = static_cast<uint32_t>(joints.size());
	joints.push_back(std::move(joint));

	for (const auto &child : node->children_) {

		ModelJointState::MakeJointIndex(child.get(), joints);
	}

	return selfIndex;
}

std::unique_ptr<Mesh> MeshFactory::CreateMesh() const
{
	std::unique_ptr<Mesh> result = std::make_unique<Mesh>();
	result->meshName_ = meshName_;
	result->pNode_ = pNode_;
	result->material_ = pMaterial_;
	result->indexBuffer_.SetIndexData(indexs_);
	result->vertexBuffer_.SetVertexData(vertices_);

	return std::move(result);
}
//
//SkinClusterReference::SkinClusterReference(uint32_t vertexCount)
//{
//	// データの確保
//	influence_.Resize(vertexCount);
//	// メモリに対するアクセスを定義
//	influenceSpan_ = { influence_.GetVertexData().data(), vertexCount };
//}
//
//std::unique_ptr<SkinClusterReference> SkinClusterReference::MakeSkinCluster(const SolEngine::ModelData *model, const SkeletonState &skeleton)
//{
//	uint32_t vertexCount = 0;
//	for (const auto &mesh : model->meshHandleList_) {
//		vertexCount += mesh->vertexBuffer_.GetVertexData().size();
//	}
//	std::unique_ptr<SkinClusterReference> result = std::make_unique<SkinClusterReference>(vertexCount);
//
//	// 番兵を取る
//	const auto jointEndIt = skeleton.reference_->jointMap_.end();
//	for (uint32_t i = 0; i < model->meshHandleList_.size(); i++) {
//
//		// そのメッシュにデータが格納されているか
//		const auto &clusterItr = model->skinCluster_->skinClusterData_.at(i);
//		if (not clusterItr) { continue; }
//		// モデルデータを解析してInfluenceを埋める
//		for (const auto &[keyName, jointWeight] : *clusterItr) {
//			// 一致するジョイントの対象が存在するか探す
//			auto it = skeleton.reference_->jointMap_.find(keyName);
//			if (it == jointEndIt) { // 存在しなかったら飛ばす
//				continue;
//			}
//
//			for (const auto &vertexWeight : jointWeight.vertexWeightData_) {
//				// 該当するinfluence情報を参照しておく
//				auto &currentInfluence = result->influenceSpan_[vertexWeight.vertexIndex_];
//				for (uint32_t index = 0; index < VertexInfluence::kNumMaxInfluence_; index++) {
//					// 空いているところにデータを代入
//					if (currentInfluence.vertexInfluence_.weight_[index] == 0.0f) {
//						currentInfluence.vertexInfluence_.weight_[index] = vertexWeight.weight_;
//						currentInfluence.vertexInfluence_.vertexIndex_[index] = it->second;
//						break;
//					}
//
//				}
//			}
//		}
//	}
//
//	return result;
//}
//
//std::unique_ptr<SkinClusterReference> SkinClusterReference::MakeSkinCluster(const Model *model, const SkeletonState &skeleton)
//{
//	uint32_t vertexCount = 0;
//	std::vector<uint32_t> vertexOffset;
//	for (const auto &mesh : model->meshList_) {
//		vertexCount += mesh->vertexBuffer_.GetVertexData().size();
//	}
//	std::unique_ptr<SkinClusterReference> result = std::make_unique<SkinClusterReference>(vertexCount);
//
//	// 番兵を取る
//	const auto jointEndIt = skeleton.reference_->jointMap_.end();
//
//	// モデルデータを解析してInfluenceを埋める
//	for (const auto &[keyName, jointWeight] : model->skinCluster_.skinClusterData_) {
//		// 一致するジョイントの対象が存在するか探す
//		auto it = skeleton.reference_->jointMap_.find(keyName);
//		if (it == jointEndIt) { // 存在しなかったら飛ばす
//			continue;
//		}
//
//		for (const auto &vertexWeight : jointWeight.vertexWeightData_) {
//			// 該当するinfluence情報を参照しておく
//			auto &currentInfluence = result->influenceSpan_[vertexWeight.vertexIndex_].vertexInfluence_;
//			for (uint32_t index = 0; index < VertexInfluence::kNumMaxInfluence_; index++) {
//				// 空いているところにデータを代入
//				if (currentInfluence.weight_[index] == 0.0f) {
//					currentInfluence.weight_[index] = vertexWeight.weight_;
//					currentInfluence.vertexIndex_[index] = it->second;
//					break;
//				}
//
//			}
//		}
//
//	}
//
//	return result;
//}


SkinCluster::SkinCluster(uint32_t jointsCount)
	:palette_(jointsCount)
{
	paletteSpan_ = { palette_.data(), jointsCount };
}

//std::unique_ptr<SkinCluster> SkinCluster::MakeSkinCluster(const Model *model, const SkeletonState &skeleton)
//{
//	std::unique_ptr<SkinCluster> result = std::make_unique<SkinCluster>(static_cast<uint32_t>(skeleton.joints_.size()));
//
//	//result->reference_ = SkinClusterReference::MakeSkinCluster(model, skeleton);
//
//	// 初期化
//	result->inverseBindPoseMatrixList_.resize(skeleton.reference_->jointMap_.size());
//	std::generate(result->inverseBindPoseMatrixList_.begin(), result->inverseBindPoseMatrixList_.end(), Matrix4x4::Identity);
//
//	const auto jointEndIt = skeleton.reference_->jointMap_.end();
//
//	// モデルデータを解析してInfluenceを埋める
//	for (const auto &[keyName, jointWeight] : model->skinCluster_.skinClusterData_) {
//		// 一致するジョイントの対象が存在するか探す
//		auto it = skeleton.reference_->jointMap_.find(keyName);
//		if (it == jointEndIt) { // 存在しなかったら飛ばす
//			continue;
//		}
//
//		// indexから、逆バインドポーズ行列を代入する
//		result->inverseBindPoseMatrixList_[it->second] = jointWeight.inverseBindPoseMatrix_;
//
//	}
//
//	return std::move(result);
//
//}

std::unique_ptr<SkinCluster> SkinCluster::MakeSkinCluster(const SolEngine::ModelData *model, const SolEngine::Skeleton *skeletonRef, const SkeletonState &skeleton)
{
	std::unique_ptr<SkinCluster> result = std::make_unique<SkinCluster>(static_cast<uint32_t>(skeleton.joints_.size()));

	result->reference_ = model;
	result->skeletonRef_ = skeletonRef;

	// 初期化
	result->inverseBindPoseMatrixList_.resize(skeleton.reference_->jointMap_.size());
	std::generate(result->inverseBindPoseMatrixList_.begin(), result->inverseBindPoseMatrixList_.end(), Matrix4x4::Identity);

	const auto jointEndIt = skeleton.reference_->jointMap_.end();


	for (uint32_t i = 0; i < model->meshHandleList_.size(); i++) {

		// そのメッシュにデータが格納されているか
		const auto &clusterItr = skeletonRef->skinCluster_->skinClusterData_.at(i);
		//if (not clusterItr) { continue; }
		// モデルデータを解析してInfluenceを埋める
		for (const auto &[keyName, jointWeight] : clusterItr) {
			// 一致するジョイントの対象が存在するか探す
			auto it = skeleton.reference_->jointMap_.find(keyName);
			if (it == jointEndIt) { // 存在しなかったら飛ばす
				continue;
			}

			// indexから、逆バインドポーズ行列を代入する
			result->inverseBindPoseMatrixList_[it->second] = jointWeight.inverseBindPoseMatrix_;

		}
	}

	return std::move(result);
}

void SkinCluster::Update(const SkeletonState &skeleton)
{

	std::transform(std::execution::par_unseq, skeleton.joints_.cbegin(), skeleton.joints_.cend(), inverseBindPoseMatrixList_.cbegin(), paletteSpan_.begin(), [](const auto &joint, const auto &ibpMat)
		{
			WellForGPU result{};

			result.skeletonSpaceMatrix = ibpMat * joint.skeletonSpaceMatrix_;
			result.skeletonSpaceInverseTransponeMatrix = result.skeletonSpaceMatrix.InverseSRT().Transpose();

			return result;
		}
	);
}

//std::unique_ptr<SkinModel> SkinModel::MakeSkinModel(Model *model)
//{
//	std::unique_ptr<SkinModel> result = std::make_unique<SkinModel>();
//
//	result->skeleton_ = SkeletonState::MakeSkeleton(model->rootNode_.get());
//	result->skinCluster_ = SkinCluster::MakeSkinCluster(model, *result->skeleton_);
//	return std::move(result);
//}

std::unique_ptr<SkinModel> SkinModel::MakeSkinModel(SolEngine::ModelData *model, SolEngine::SkeletonAnimation::Skeleton *skeleton)
{
	std::unique_ptr<SkinModel> result = std::make_unique<SkinModel>();

	result->skeleton_ = SkeletonState::MakeSkeleton(skeleton);
	result->skinCluster_ = SkinCluster::MakeSkinCluster(model, skeleton, *result->skeleton_);
	return std::move(result);
}

void SkinModel::Update(const SolEngine::SkeletonAnimation::Animation &animation, const float animateTime)
{
	skeleton_->ApplyAnimation(animation, animateTime);
	skeleton_->UpdateMatrix();
	skinCluster_->Update(*skeleton_);
}

void SkeletonState::AddDrawBuffer(const Matrix4x4 &transMat, const Vector3 &drawOffset) const
{
	auto *blockRender_ = BlockManager::GetInstance();
	Model *plane = ModelManager::GetInstance()->GetModel("Plane");
	const Matrix4x4 &cameraInvMat = SolEngine::CameraManager::GetInstance()->GetCamera()->matProjection_.InverseSRT();

	for (const auto &joint : joints_) {

		Matrix4x4 affineMat = cameraInvMat;
		std::for_each_n(affineMat.begin(), 12, [](float &v) {v *= 0.1f; });
		/*for (uint32_t i = 0; i < 12; i++) {
			affineMat.arr[i] = affineMat.arr[i] * 0.1f;
		}*/
		affineMat.GetTranslate() = joint.skeletonSpaceMatrix_.GetTranslate() * transMat + drawOffset;
		blockRender_->AddBox(plane, { .transMat_ = affineMat });
	}

}
