#include "Create.h"
#include "../Render/Render.h"
#include "../../../Utils/Math/Math.hpp"
#include "../../../Utils/Math/Vector3.h"

#include <numbers>
#include <wrl.h>

Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(ID3D12Device *device, const size_t &sizeBytes)
{
	// ヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
	// リソースの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	// バッファリソース。 テクスチャの場合はまた別の設定をする
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Width = sizeBytes;					// リソースのサイズ。
	// バッファの場合はこれらは1にする決まり
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.SampleDesc.Count = 1;
	// バッファの場合はこれにする決まり
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	// 実際に頂点リソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> result = nullptr;
	HRESULT hr = S_FALSE;
	hr = device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&result));
	assert(SUCCEEDED(hr));
	return result;
}

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(ID3D12Device *device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible)
{
	// ディスクリプタヒープの生成
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = heapType;
	descriptorHeapDesc.NumDescriptors = numDescriptors;
	descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	HRESULT hr = S_FALSE;
	hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
	// デスクリプタヒープが生成できなかったので起動できない
	assert(SUCCEEDED(hr));
	return descriptorHeap;
}
