#pragma once
#include "../Base/EngineObject.h"
#include <d3d12.h>
#include "../Base/DirectXCommon.h"

namespace SolEngine {
	class SkinningComputeShader : protected EngineObject {
	public:

		SkinningComputeShader() = default;

		bool operator==(const SkinningComputeShader &other) const = default;

	public:

		void Init();


	private:

		ComPtr<ID3D12PipelineState> computePipelineState_;

	};


}