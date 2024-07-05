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
		ID3D12Resource *GetResource() { return renderTargetTexture_.Get(); }

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

	class ShaderEffectProcessor : public SoLib::Singleton<ShaderEffectProcessor>, protected SolEngine::EngineObject {
		friend SoLib::Singleton<ShaderEffectProcessor>;
		ShaderEffectProcessor() = default;
		ShaderEffectProcessor(const ShaderEffectProcessor &) = delete;
		ShaderEffectProcessor &operator=(const ShaderEffectProcessor &) = delete;
		~ShaderEffectProcessor() = default;

	public:

		void Init();

		void Input(ID3D12Resource *const resource);

		void GetResult(ID3D12Resource *const result) const;

		static void CopyTexture(ID3D12Resource *const src, ID3D12Resource *const dst);

		template<SoLib::IsRealType... Ts>
		void Execute(const std::wstring &psName, const CBuffer<Ts> &...args);
		/*template<SoLib::IsRealType...Ts>
			requires(sizeof...(Ts) > 1)
		void Execute(const CBuffer<Ts>&... args) {



		}*/


	private:

		std::array<ComPtr<ID3D12Resource>, 2u> fullScreenTexture_;
		std::unique_ptr<DescHeap<D3D12_DESCRIPTOR_HEAP_TYPE_RTV>> rtvDescHeap_;

		uint32_t textureTarget_ = 0u;

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

		const SolEngine::RootParametersAccesser<DescHeapCbvSrvUav::Handle, CBuffer<ValuePair>> accesser_ = SolEngine::MakeRootParametersAccesser(
			SignParam<DescHeapCbvSrvUav::Handle>{ "t0PS" },
			SignParam<CBuffer<ValuePair>>{ "b0PS" }
		);

		CBuffer<ValuePair> param_;

		SolEngine::ResourceObjectManager<RootSignature>::Handle rootSignature_;

		std::map<std::pair<std::wstring, std::wstring>, ComPtr<ID3D12PipelineState>> pipelineState_;

	};

	template<SoLib::IsRealType... Ts>
	inline void ShaderEffectProcessor::Execute(const std::wstring &psName, const CBuffer<Ts> &...args) {

		FullScreenRenderer *const renderer = FullScreenRenderer::GetInstance();

		auto command = GetCommandList();

		// 表のテクスチャを指定
		const auto &beforeTexture = rtvDescHeap_->GetHandle(0, textureTarget_);

		// 裏のテクスチャを指定
		const auto &afterTexture = fullScreenTexture_[(textureTarget_ + 1) % 2];

		const std::pair<std::wstring, std::wstring> &key = { L"FullScreen.VS.hlsl" ,psName };

#pragma region TransitionBarrierを張る

		// TransitionBarrierの設定
		D3D12_RESOURCE_BARRIER barrierF{};
		// 今回のバリアはTransition
		barrierF.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		// Noneにしておく
		barrierF.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		// バリアを張る対象のリソース。
		barrierF.Transition.pResource = afterTexture.Get();
		// 遷移前(現在)のResourceState
		barrierF.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		// 遷移後のResourceState
		barrierF.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		// TransitionBurrierを張る
		command->ResourceBarrier(1, &barrierF);

#pragma endregion


		command->SetGraphicsRootSignature(renderer->GetRootSignature());
		command->SetPipelineState(renderer->GetPipeLine(key));
		command->SetGraphicsRootDescriptorTable(0, beforeTexture.gpuHandle_);

		uint32_t index = 1u;
		// 展開を使って各argに対してSetGraphicsRootConstantBufferViewを呼び出す
		(command->SetGraphicsRootConstantBufferView(index++, args.GetGPUVirtualAddress()));

		//command->SetGraphicsRootConstantBufferView(1, args.GetGPUVirtualAddress());

		command->DrawInstanced(3, 1, 0, 0);


#pragma region TransitionBarrierを張る

		// TransitionBarrierの設定
		D3D12_RESOURCE_BARRIER barrierS{};
		// 今回のバリアはTransition
		barrierS.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		// Noneにしておく
		barrierS.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		// バリアを張る対象のリソース。
		barrierS.Transition.pResource = afterTexture.Get();
		// 遷移前(現在)のResourceState
		barrierS.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		// 遷移後のResourceState
		barrierS.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		// TransitionBurrierを張る
		command->ResourceBarrier(1, &barrierS);

#pragma endregion

		// ターゲットを進める
		textureTarget_ = (textureTarget_ + 1) % 2;

	}

}