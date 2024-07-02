#pragma once
#include "../Base/EngineObject.h"
#include <d3d12.h>
#include "../Base/DirectXCommon.h"
#include "../Base/StructBuffer.h"
#include "../Model/Mesh.h"
#include "../Model/ModelData.h"

namespace SolEngine {
	class SkinningComputeShader : protected EngineObject {
	public:

		SkinningComputeShader() = default;

		bool operator==(const SkinningComputeShader &other) const = default;

	public:

		void Init(const ResourceObjectManager<ModelData>::Handle modelData);


	private:

		std::vector<std::unique_ptr<ArrayBuffer<Mesh::VertexData, D3D12_HEAP_TYPE_DEFAULT>>> outputVertex_;

		//ArrayBuffer<Mesh::VertexData> outPutData_;

		DescHeapCbvSrvUav::HeapRange heapRange_;
		ComPtr<ID3D12PipelineState> computePipelineState_;

		D3D12_UNORDERED_ACCESS_VIEW_DESC uavView_;

	};


}