/// @file OffScreenRendering.h
/// @brief オフスクリーンの描画クラス
/// @author ONUKI seiya
#pragma once
#include "../../Engine/Utils/Containers/Singleton.h"
#include "../../Engine/Utils/Graphics/Color.h"
#include "../../ResourceObject/ResourceObjectManager.h"
#include "../Base/CBuffer.h"
#include "../Base/DirectXCommon.h"
#include "../Base/EngineObject.h"
#include "../Base/RootSignature.h"
#include <d3d12.h>
#include <wrl.h>

namespace PostEffect {

	class OffScreenRenderer : public SolEngine::EngineObject
	{
		// friend SoLib::Singleton<OffScreenRenderer>;

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

		/// @brief 初期化する色を取得
		/// @return 初期化する色
		inline const SoLib::Color::RGB4 &GetClearColor() { return clearColor_; }

		const DescHeapCbvSrvUav::HeapRange *const GetHeapRange() const { return &srvHeapRange_; }

		// private:

		/// @brief レンダーターゲットを作成する
		/// @param[in] device デバイス
		/// @param[in] width 横幅
		/// @param[in] height 高さ
		/// @param[in] format フォーマット
		/// @param[in] clearColor 初期化する色
		static ComPtr<ID3D12Resource> CreateRenderTextrueResource(ID3D12Device *device, uint32_t width, uint32_t height, DXGI_FORMAT format, const SoLib::Color::RGB4 &clearColor);

	private:
		DirectResourceLeakChecker leakChecker_{};

		// クリア時の色
		SoLib::Color::RGB4 clearColor_ = 0xFF0000FF; // 赤を指定しておく

		ComPtr<ID3D12Resource> renderTargetTexture_{};

		std::unique_ptr<DescHeap<D3D12_DESCRIPTOR_HEAP_TYPE_RTV>> rtvDescHeap_ = nullptr;

		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_{};

		DescHeapCbvSrvUav::HeapRange srvHeapRange_{};
	};

	/// @brief シェーダーエフェクト処理クラス
	class ShaderEffectProcessor : public SoLib::Singleton<ShaderEffectProcessor>, protected SolEngine::EngineObject
	{
		friend SoLib::Singleton<ShaderEffectProcessor>;
		ShaderEffectProcessor() = default;
		ShaderEffectProcessor(const ShaderEffectProcessor &) = delete;
		ShaderEffectProcessor &operator=(const ShaderEffectProcessor &) = delete;
		~ShaderEffectProcessor() = default;

	public:
		static constexpr uint32_t kTextureCount_ = 3u;

		/// @brief 初期化
		void Init();

		/// @brief テクスチャを入力する
		/// @param[in] resource 入力するテクスチャ
		/// @details RenderTargetTextureを入力する
		void Input(ID3D12Resource *const resource);

		/// @brief 変更したテクスチャを取得
		/// @param[out] result 出力先のテクスチャ
		/// @details RenderTargetTextureに出力する
		void GetResult(ID3D12Resource *const result) const;

		/// @brief テクスチャをコピー
		/// @param[in] src コピー元のテクスチャ
		/// @param[in] dst コピー先のテクスチャ
		/// @details コピー元のテクスチャをコピー先のテクスチャにコピーする｡\
		/// RenderTargetTextureである必要がある｡
		static void CopyTexture(ID3D12Resource *const src, ID3D12Resource *const dst);

		/// @brief シェーダーを実行する
		/// @param[in] psName シェーダー名
		/// @param[in] args シェーダーに渡す引数
		template <SoLib::IsRealType... Ts>
		void Execute(const std::wstring &psName, const CBuffer<Ts> &...args);

	private:
		std::array<ComPtr<ID3D12Resource>, kTextureCount_> fullScreenTexture_;
		std::unique_ptr<DescHeap<D3D12_DESCRIPTOR_HEAP_TYPE_RTV>> rtvDescHeap_;

		uint32_t textureTarget_ = 0u;

		DescHeapCbvSrvUav::HeapRange srvHeapRange_;
	};

	class FullScreenRenderer : public SoLib::Singleton<FullScreenRenderer>, public SolEngine::EngineObject
	{
		friend SoLib::Singleton<FullScreenRenderer>;

		FullScreenRenderer() = default;
		FullScreenRenderer(const FullScreenRenderer &) = delete;
		FullScreenRenderer &operator=(const FullScreenRenderer &) = delete;
		~FullScreenRenderer() = default;

	private:
	public:
		/// @brief 初期化
		/// @param[in] key シェーダー名
		void Init(const std::list<std::wstring> &key);
		/// @brief 描画
		/// @param[in] key シェーダー名
		/// @param[in] texture 描画するテクスチャ
		/// @param[in] gpuHandle 描画するテクスチャのGPUハンドル
		void Draw(const std::wstring &key, ID3D12Resource *texture, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle);

		/// @brief ルートシグネチャを取得
		ID3D12RootSignature *GetRootSignature() { return rootSignature_->Get(); }
		/// @brief パイプラインステートを取得
		ID3D12PipelineState *GetPipeLine(const std::wstring &key) { return pipelineState_.at(key).Get(); }

	private:

		std::unique_ptr<RootSignature> rootSignature_;

		std::map<std::wstring, ComPtr<ID3D12PipelineState>> pipelineState_;
	};

	template <SoLib::IsRealType... Ts>
	inline void ShaderEffectProcessor::Execute(const std::wstring &psName, const CBuffer<Ts> &...args)
	{

		FullScreenRenderer *const renderer = FullScreenRenderer::GetInstance();
		auto *const pDxCommon = DirectXCommon::GetInstance();

#pragma region ViewportとScissor(シザー)

		// ビューポート
		D3D12_VIEWPORT viewport;
		// シザー短形
		D3D12_RECT scissorRect{};

		pDxCommon->SetFullscreenViewPort(&viewport, &scissorRect);

#pragma endregion

		auto command = GetCommandList();

		// 元々のテクスチャを指定
		const auto &beforeTextureSrv = srvHeapRange_.GetHandle(textureTarget_);
		const auto &beforeTexture = fullScreenTexture_[textureTarget_];

		// 書き込み先
		auto targetTexture = rtvDescHeap_->GetHandle(0, (textureTarget_ + 1) % kTextureCount_);

		// 検索名
		const auto &key = psName;

#pragma region TransitionBarrierを張る

		// TransitionBarrierの設定
		D3D12_RESOURCE_BARRIER barrierF{};
		// 今回のバリアはTransition
		barrierF.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		// Noneにしておく
		barrierF.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		// バリアを張る対象のリソース。
		barrierF.Transition.pResource = beforeTexture.Get();
		// 遷移前(現在)のResourceState
		barrierF.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		// 遷移後のResourceState
		barrierF.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		// TransitionBurrierを張る
		command->ResourceBarrier(1, &barrierF);

#pragma endregion

		pDxCommon->DrawTargetReset(&targetTexture.cpuHandle_, 0xFF0000FF, nullptr, viewport, scissorRect);

		command->SetGraphicsRootSignature(renderer->GetRootSignature());
		command->SetPipelineState(renderer->GetPipeLine(key));
		command->SetGraphicsRootDescriptorTable(0, beforeTextureSrv.gpuHandle_);

		// もしバッファが渡されていたら
		if constexpr (sizeof...(Ts) != 0u) {
			uint32_t index = 1u;
			// 展開を使って各argに対してSetGraphicsRootConstantBufferViewを呼び出す
			(command->SetGraphicsRootConstantBufferView(index++, args.GetGPUVirtualAddress()), ...);
		}

		// command->SetGraphicsRootConstantBufferView(1, args.GetGPUVirtualAddress());

		command->DrawInstanced(3, 1, 0, 0);

#pragma region TransitionBarrierを張る

		// TransitionBarrierの設定
		D3D12_RESOURCE_BARRIER barrierS{};
		// 今回のバリアはTransition
		barrierS.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		// Noneにしておく
		barrierS.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		// バリアを張る対象のリソース。
		barrierS.Transition.pResource = beforeTexture.Get();
		// 遷移前(現在)のResourceState
		barrierS.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		// 遷移後のResourceState
		barrierS.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		// TransitionBurrierを張る
		command->ResourceBarrier(1, &barrierS);

#pragma endregion

		// ターゲットを進める
		textureTarget_ = (textureTarget_ + 1) % kTextureCount_;
	}

}