/// @file SkinningComputeShader.h
/// @brief スキニングをコンピュートシェーダーで行う
/// @author ONUKI seiya
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

		/// @brief 初期化
		/// @param[in] vertexCount 頂点数
		void Init(const uint32_t vertexCount);

		/// @brief Update
		/// @param[in] skinCluster スキニングの情報群
		/// @param[in] modelData モデルデータ
		/// @param[in] modelInfluence モデルへの影響量
		void Update(const ::SkinCluster &skinCluster, const ModelData *modelData, const ModelInfluence *modelInfluence);

		/// @brief データを取得
		/// @return 計算し終わったデータ
		const auto &GetOutputData() const { return outputData_; }

	private:

		VertexBuffer<ModelVertexData::VertexData, D3D12_HEAP_TYPE_DEFAULT> outputData_;

		DescHeapCbvSrvUav::HeapRange heapRange_;
		ComPtr<ID3D12PipelineState> computePipelineState_;
		ResourceObjectManager<RootSignature>::Handle rootSignature_;


		D3D12_UNORDERED_ACCESS_VIEW_DESC uavView_;

	};


}