#pragma once
#include "../../../Utils/Containers/Singleton.h"
#include "../../../Utils/Graphics/Color.h"
#include "../Base/DirectXCommon.h"
#include <d3d12.h>
#include <wrl.h>
#include "../Base/EngineObject.h"
#include "../Base/RootSignature.h"

namespace PostEffect {

	class OffScreenRenderer : public SoLib::Singleton<OffScreenRenderer>, public SolEngine::EngineObject {
		friend SoLib::Singleton<OffScreenRenderer>;

		OffScreenRenderer() = default;
		OffScreenRenderer(const OffScreenRenderer &) = delete;
		OffScreenRenderer &operator=(const OffScreenRenderer &) = delete;
		~OffScreenRenderer() = default;

	public:
		void Init();

		/// @brief テクスチャの取得
		/// @return テクスチャリソース
		ID3D12Resource *GetTexture() { return renderTargetTexture_.Get(); }

		/// @brief RTVのデスクリプタヒープ
		/// @return rtvヒープのポインタ
		DescHeap<D3D12_DESCRIPTOR_HEAP_TYPE_RTV> *GetRtvDescHeap() { return rtvDescHeap_.get(); }

		const SoLib::Color::RGB4 &GetClearColor() const { return clearColor_; }

		const DescHeapCbvSrvUav::HeapRange *const GetHeapRange() const { return &srvHeapRange_; }

	private:
		static ComPtr<ID3D12Resource> CreateRenderTextrueResource(ID3D12Device *device, uint32_t width, uint32_t height, DXGI_FORMAT format, const SoLib::Color::RGB4 &clearColor);

	private:

		DirectResourceLeakChecker leakChecker_{};

		// クリア時の色
		const SoLib::Color::RGB4 &clearColor_ = 0xFF0000FF; // 赤を指定しておく

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

		void Init();

		void Draw(ID3D12Resource *texture, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle);

		ID3D12RootSignature *GetRootSignature() { return rootSignature_.Get(); }

		ID3D12PipelineState *GetPipeLine() { return pipelineState_.Get(); }

	private:

		RootSignature rootSignature_;

		ComPtr<ID3D12PipelineState> pipelineState_ = nullptr;



	};

}