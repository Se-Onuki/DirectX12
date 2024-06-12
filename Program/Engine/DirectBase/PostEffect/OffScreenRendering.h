#pragma once
#include "../../../Utils/Containers/Singleton.h"
#include "../../../Utils/Graphics/Color.h"
#include "../Base/DirectXCommon.h"
#include <d3d12.h>
#include <wrl.h>
#include "../Base/EngineObject.h"
#include "../Base/RootSignature.h"
#include "../Base/CBuffer.h"
#include "../../ResourceObject/ResourceObjectManager.h"

namespace PostEffect {

	class OffScreenRenderer : public SolEngine::EngineObject {
		//friend SoLib::Singleton<OffScreenRenderer>;

	public:
		OffScreenRenderer() = default;
		/*OffScreenRenderer(const OffScreenRenderer &) = delete;
		OffScreenRenderer &operator=(const OffScreenRenderer &) = delete;*/
		~OffScreenRenderer() = default;

		void Init();

		/// @brief テクスチャの取得
		/// @return テクスチャリソース
		ID3D12Resource *GetTexture() { return renderTargetTexture_.Get(); }

		/// @brief RTVのデスクリプタヒープ
		/// @return rtvヒープのポインタ
		DescHeap<D3D12_DESCRIPTOR_HEAP_TYPE_RTV> *GetRtvDescHeap() { return rtvDescHeap_.get(); }

		inline const SoLib::Color::RGB4 &GetClearColor() { return clearColor_; }

		const DescHeapCbvSrvUav::HeapRange *const GetHeapRange() const { return &srvHeapRange_; }

		//private:
		static ComPtr<ID3D12Resource> CreateRenderTextrueResource(ID3D12Device *device, uint32_t width, uint32_t height, DXGI_FORMAT format, const SoLib::Color::RGB4 &clearColor);

	private:

		DirectResourceLeakChecker leakChecker_{};

		// クリア時の色
		SoLib::Color::RGB4 clearColor_ = 0xFF0000FF; // 赤を指定しておく

		ComPtr<ID3D12Resource> renderTargetTexture_;

		std::unique_ptr<DescHeap<D3D12_DESCRIPTOR_HEAP_TYPE_RTV>> rtvDescHeap_;

		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_;

		DescHeapCbvSrvUav::HeapRange srvHeapRange_;


	};

	class FullScreenRenderer : public SoLib::Singleton<FullScreenRenderer>, public SolEngine::EngineObject {
		friend SoLib::Singleton<FullScreenRenderer>;

		FullScreenRenderer() = default;
		FullScreenRenderer(const FullScreenRenderer &) = delete;
		FullScreenRenderer &operator=(const FullScreenRenderer &) = delete;
		~FullScreenRenderer() = default;

	public:

		void Init(const std::list<std::pair<std::wstring, std::wstring>> &key);

		void Draw(const std::pair<std::wstring, std::wstring> &key, ID3D12Resource *texture, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle);

		ID3D12RootSignature *GetRootSignature() { return rootSignature_.GetResource()->Get(); }

		ID3D12PipelineState *GetPipeLine(const std::pair<std::wstring, std::wstring> &key) { return pipelineState_.at(key).Get(); }

		std::pair<float, float> *GetFParam() { return &param_->fValue_; }
		std::pair<float, int32_t> *GetGaussianParam() { return &param_->iValue_; }

	private:
		union ValuePair {
			std::pair<float, float> fValue_;
			std::pair<float, int32_t> iValue_;
		};

		CBuffer<ValuePair> param_;

		SolEngine::ResourceObjectManager<RootSignature>::Handle rootSignature_;

		std::map<std::pair<std::wstring, std::wstring>, ComPtr<ID3D12PipelineState>> pipelineState_;

	};

}