/// @file Create.h
/// @brief DirectResourceを作成する
/// @author ONUKI seiya
#pragma once
#include <d3d12.h>
#include <cassert>
#include <cstdint>
#include <wrl.h>

/// @brief リソースを作成する
/// @param[in] device デバイス
/// @param[in] sizeBytes リソースサイズ
/// @param[in] heapType ヒープの種類
/// @return 生成されたリソース
Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(ID3D12Device *device, const size_t sizeBytes, const D3D12_HEAP_TYPE heapType = D3D12_HEAP_TYPE_UPLOAD);

/// @brief デスクリプタヒープを作成する
/// @param[in] device デバイス
/// @param[in] heapType ヒープの種類
/// @param[in] numDescriptors デスクリプタの数
/// @param[in] shaderVisible シェーダーから見えるか
/// @return 生成されたヒープ
[[nodiscard]] Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>CreateDescriptorHeap(ID3D12Device *device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);
