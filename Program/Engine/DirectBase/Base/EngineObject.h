/// @file EngineObject.h
/// @brief DirectXCommonとの結合度を下げるための基底クラス
/// @author ONUKI seiya
#pragma once
#include "../Descriptor/DescriptorManager.h"
#include <cstdint>
#include <d3d12.h>
#include <wrl.h>

///
///	DirectXCommonとの結合度を下げるための基底クラス
///

namespace SolEngine {

	/// @brief エンジンオブジェクト
	class EngineObject
	{
		friend class DirectXCommon;

		// DirectX12デバイスを保持するポインタ
		inline static ID3D12Device *device_ = nullptr;

		// srvのヒープを保持するポインタ
		inline static DescHeapCbvSrvUav *srvHeap_ = nullptr;

		// rtvのヒープを保持するポインタ
		inline static DescHeap<D3D12_DESCRIPTOR_HEAP_TYPE_RTV> *rtvHeap_ = nullptr;

		inline static ID3D12GraphicsCommandList *commandList_ = nullptr;

		inline static uint32_t descriptorSizeSRV_ = 0;
		inline static uint32_t descriptorSizeRTV_ = 0;
		inline static uint32_t descriptorSizeDSV_ = 0;

	protected:
		/// @brief ComPtrのラッパー
		/// @tparam T 格納する型
		template <typename T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;

		/// @fn static ID3D12Device *GetDevice(void)
		/// @brief DirectXデバイスへのアクセッサ
		/// @return ID3D12Deviceポインタ
		static ID3D12Device *GetDevice() { return device_; }

		/// @fn static DescHeapCbvSrvUav *GetDescHeapCbvSrvUav(void)
		/// @brief SRVヒープへのアクセッサ
		/// @return SRVヒープ
		static DescHeapCbvSrvUav *GetDescHeapCbvSrvUav() { return srvHeap_; }
		/// @fn static DescHeap<D3D12_DESCRIPTOR_HEAP_TYPE_RTV> *GetDescHeapRtv(void)
		/// @brief RTVヒープへのアクセッサ
		/// @return RTVヒープ
		static DescHeap<D3D12_DESCRIPTOR_HEAP_TYPE_RTV> *GetDescHeapRtv() { return rtvHeap_; }
		/// @fn static ID3D12GraphicsCommandList *GetCommandList(void)
		/// @brief コマンドリストへのアクセッサ
		/// @return コマンドリスト
		static ID3D12GraphicsCommandList *GetCommandList() { return commandList_; }

		/// @fn static uint32_t GetDescSize<D3D12_DESCRIPTOR_HEAP_TYPE>()
		/// @brief ディスクリプターサイズの取得
		/// @tparam HeapType ヒープタイプ
		/// @return ディスクリプターサイズ
		/*template <D3D12_DESCRIPTOR_HEAP_TYPE HeapType>
		static uint32_t GetDescSize();*/

		/// @fn static uint32_t GetDescriptorSizeSRV(void)
		/// @brief SRVヒープサイズの取得
		/// @return SRVのヒープサイズ
		static uint32_t GetDescriptorSizeSRV() { return descriptorSizeSRV_; }
		
		/// @fn static uint32_t GetDescriptorSizeRTV(void)
		/// @brief RTVヒープサイズの取得
		/// @return RTVのヒープサイズ
		static uint32_t GetDescriptorSizeRTV() { return descriptorSizeRTV_; }
		
		/// @fn static uint32_t GetDescriptorSizeDSV(void)
		/// @brief DSVヒープサイズの取得
		/// @return DSVのヒープサイズ
		static uint32_t GetDescriptorSizeDSV() { return descriptorSizeDSV_; }
	};

}