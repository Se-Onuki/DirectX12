#pragma once
#include "../Base/EngineObject.h"
#include <d3d12.h>
#include "../Base/DirectXCommon.h"
#include "../Base/StructBuffer.h"
#include "../Model/Mesh.h"

namespace SolEngine {
	class SkinningComputeShader : protected EngineObject {
	public:

		SkinningComputeShader() = default;

		bool operator==(const SkinningComputeShader &other) const = default;

	public:

		void Init(const uint32_t vertexCount);


	private:

		ArrayBuffer<ModelVertexData::VertexData> outPutData_;

		DescHeapCbvSrvUav::HeapRange heapRange_;
		ComPtr<ID3D12PipelineState> computePipelineState_;

		D3D12_UNORDERED_ACCESS_VIEW_DESC uavView_;

	};


}