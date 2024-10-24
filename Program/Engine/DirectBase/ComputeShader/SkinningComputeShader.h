#pragma once
#include "../Base/EngineObject.h"
#include <d3d12.h>
#include "../Base/DirectXCommon.h"
#include "../Base/StructBuffer.h"
#include "../Model/Model.h"
#include "../Model/ModelData.h"
#include "../Model/ModelVertexData.h"
#include "../Model/ModelInfluence.h"

namespace SolEngine {
	class SkinningComputeShader : protected EngineObject {
	public:

		SkinningComputeShader() = default;

		bool operator==(const SkinningComputeShader &other) const = default;

	public:

		void Init(const uint32_t vertexCount);

		void Update(const ::SkinCluster &skinCluster, const ModelData *modelData, const ModelInfluence *modelInfluence);

		const auto &GetOutputData() const { return outputData_; }

	private:

		VertexBuffer<ModelVertexData::VertexData, D3D12_HEAP_TYPE_DEFAULT> outputData_;

		DescHeapCbvSrvUav::HeapRange heapRange_;
		ComPtr<ID3D12PipelineState> computePipelineState_;
		ResourceObjectManager<RootSignature>::Handle rootSignature_;


		D3D12_UNORDERED_ACCESS_VIEW_DESC uavView_;

	};


}