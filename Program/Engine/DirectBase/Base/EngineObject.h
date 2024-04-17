#pragma once
#include <cstdint>
#include <d3d12.h>
#include <wrl.h>

///
///	DirectXCommonとの結合度を下げるための基底クラス
///

namespace SolEngine {

	/// @brief エンジンオブジェクト
	class EngineObject {
		friend class DirectXCommon;

		// DirectX12デバイスを保持するポインタ
		inline static ID3D12Device *device_ = nullptr;

		// srvのヒープを保持するポインタ
		inline static DescHeapCbvSrvUav *srvHeap_ = nullptr;

		inline static ID3D12CommandList *commandList_ = nullptr;

		inline static uint32_t descriptorSizeSRV_ = 0;
		inline static uint32_t descriptorSizeRTV_ = 0;
		inline static uint32_t descriptorSizeDSV_ = 0;

	protected:

		/// @brief ComPtrのラッパー
		/// @tparam T 格納する型
		template <typename T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;

		/// @brief DirectXデバイスへのアクセッサ
		/// @return ID3D12Deviceポインタ
		static ID3D12Device *GetDevice() { return device_; }

		static uint32_t GetDescriptorSizeSRV() { return descriptorSizeSRV_; }
		static uint32_t GetDescriptorSizeRTV() { return descriptorSizeRTV_; }
		static uint32_t GetDescriptorSizeDSV() { return descriptorSizeDSV_; }
	};

}