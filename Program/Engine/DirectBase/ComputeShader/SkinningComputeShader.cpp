#include "SkinningComputeShader.h"
#include "../Base/Shader.h"
#include "../Base/RootSignature.h"

namespace SolEngine {

	void SkinningComputeShader::Init(const uint32_t vertexCount)
	{
		auto device = GetDevice();
		auto srvHeap = GetDescHeapCbvSrvUav();

		D3D12_COMPUTE_PIPELINE_STATE_DESC computePipelineStateDesc{};

		ResourceObjectManager<Shader> *const shaderManager = ResourceObjectManager<Shader>::GetInstance();
		auto shader = shaderManager->Load({ .name_ = L"Skinning.CS.hlsl", .profile_ = L"cs_6_0" });

		ResourceObjectManager<RootSignature> *const rootSignatureManager = ResourceObjectManager<RootSignature>::GetInstance();
		auto rootSignature = rootSignatureManager->Load({ .item_ = {"t0AL,t1AL,t2AL,u0AL,b0AL"} });

		computePipelineStateDesc.CS = shader->GetBytecode();
		computePipelineStateDesc.pRootSignature = rootSignature->Get();

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

		heapRange_ = srvHeap->RequestHeapAllocation(1u);
		device->CreateUnorderedAccessView(outPutData_.GetResources(), nullptr, &uavView_, heapRange_.GetHandle(0u).cpuHandle_);

	}
}