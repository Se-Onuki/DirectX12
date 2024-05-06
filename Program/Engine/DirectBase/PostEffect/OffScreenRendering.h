#pragma once
#include "../../../Utils/Containers/Singleton.h"
#include "../../../Utils/Graphics/Color.h"
#include "../Base/DirectXCommon.h"
#include <d3d12.h>
#include <wrl.h>
#include "../Base/EngineObject.h"

namespace PostEffect {

	class OffScreenRenderer : public SoLib::Singleton<OffScreenRenderer>, public SolEngine::EngineObject {
		friend SoLib::Singleton<OffScreenRenderer>;

		OffScreenRenderer() = default;
		OffScreenRenderer(const OffScreenRenderer &) = delete;
		OffScreenRenderer &operator=(const OffScreenRenderer &) = delete;
		~OffScreenRenderer() = default;

		/*template <typename T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;*/

	public:
		void Init();

	private:
		static ComPtr<ID3D12Resource> CreateRenderTextrueResource(ID3D12Device *device, uint32_t width, uint32_t height, DXGI_FORMAT format, const SoLib::Color::RGB4 &clearColor);

	private:

		DirectResourceLeakChecker leakChecker_{};

		// クリア時の色
		const SoLib::Color::RGB4 &clearColor_ = 0xFF0000FF; // 赤を指定しておく

		// DescHeapCbvSrvUav::HeapRange srvHeapRange_;

		ComPtr<ID3D12Resource> renderTargetTexture_;

		std::unique_ptr<DescHeap<D3D12_DESCRIPTOR_HEAP_TYPE_RTV>> rtvDescHeap_;

		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_;

		DescHeapCbvSrvUav::HeapRange srvHeapRange_;


	};

}