/// @file SkinningComputeShader.cpp
/// @brief スキニングをコンピュートシェーダーで行う
/// @author ONUKI seiya
#include "SkinningComputeShader.h"
#include "../Base/Shader.h"
#include "../Base/RootSignature.h"
#include "../Base/DirectXCommon.h"

namespace SolEngine {

	void SkinningComputeShader::Init(const uint32_t vertexCount)
	{
		auto device = GetDevice();
		auto srvHeap = GetDescHeapCbvSrvUav();

		D3D12_COMPUTE_PIPELINE_STATE_DESC computePipelineStateDesc{};

		ResourceObjectManager<Shader> *const shaderManager = ResourceObjectManager<Shader>::GetInstance();
		auto shader = shaderManager->Load({ .name_ = L"Skinning.CS.hlsl", .profile_ = L"cs_6_0" });

		ResourceObjectManager<RootSignature> *const rootSignatureManager = ResourceObjectManager<RootSignature>::GetInstance();
		rootSignature_ = rootSignatureManager->Load({ .item_ = {"t0AL,t1AL,t2AL,u0AL,b0AL"} });

		computePipelineStateDesc.CS = shader->GetBytecode();
		computePipelineStateDesc.pRootSignature = rootSignature_->Get();

		HRESULT hr = S_FALSE;

		hr = device->CreateComputePipelineState(&computePipelineStateDesc, IID_PPV_ARGS(&computePipelineState_));

		assert(SUCCEEDED(hr));

		uavView_ = {
			.Format = DXGI_FORMAT_UNKNOWN,
			.ViewDimension = D3D12_UAV_DIMENSION_BUFFER,
			.Buffer = {
				.FirstElement = 0,
				.NumElements = vertexCount,			// 頂点数で計算を行う
				.StructureByteStride = sizeof(ModelVertexData::VertexData),
				.CounterOffsetInBytes = 0,
				.Flags = D3D12_BUFFER_UAV_FLAG_NONE,
			},
		};

		outputData_.Resize(vertexCount);

		heapRange_ = srvHeap->RequestHeapAllocation(1u);
		device->CreateUnorderedAccessView(outputData_.GetVertexData().GetResources(), nullptr, &uavView_, heapRange_.GetHandle(0u).cpuHandle_);

		auto *const dxCommon = ::DirectXCommon::GetInstance();
		auto *const commList = dxCommon->GetCommandList();


		// リソースバリアを使用して状態を変更
		D3D12_RESOURCE_BARRIER resourceBarrier = {};
		resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		resourceBarrier.Transition.pResource = outputData_.GetVertexData().GetResources();
		resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;

		commList->ResourceBarrier(1, &resourceBarrier);


	}

	void SkinningComputeShader::Update(const::SkinCluster &skinCluster, const ModelData *modelData, const ModelInfluence *modelInfluence)
	{
		auto *const dxCommon = ::DirectXCommon::GetInstance();
		auto *const commList = dxCommon->GetCommandList();


		// リソースバリアを使用して状態を変更
		D3D12_RESOURCE_BARRIER resourceBarrier = {};
		resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		resourceBarrier.Transition.pResource = outputData_.GetVertexData().GetResources();
		resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_GENERIC_READ;
		resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;

		commList->ResourceBarrier(1, &resourceBarrier);		 

		commList->SetComputeRootSignature(rootSignature_->Get());
		commList->SetPipelineState(computePipelineState_.Get());

		dxCommon->GetSRVHeap()->SetCommand(commList, 1);

		commList->SetComputeRootDescriptorTable(0, skinCluster.GetPalette().GetHeapRange().GetHandle().gpuHandle_);
		commList->SetComputeRootDescriptorTable(1, modelData->modelVertex_->heapRange_.GetHandle().gpuHandle_);
		commList->SetComputeRootDescriptorTable(2, modelInfluence->heapRange_.GetHandle().gpuHandle_);
		commList->SetComputeRootDescriptorTable(3, heapRange_.GetHandle().gpuHandle_);

		commList->SetComputeRootConstantBufferView(4, modelInfluence->vertexCount_.GetGPUVirtualAddress());

		commList->Dispatch(static_cast<uint32_t>(modelInfluence->influence_.GetVertexData().size() + 1023) / 1024, 1, 1);

		resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;

		commList->ResourceBarrier(1, &resourceBarrier);

	}
}