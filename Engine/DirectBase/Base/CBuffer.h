#pragma once
#include <wrl.h>
#include <d3d12.h>

#include "../Create/Create.h"
#include "DirectXCommon.h"

template<typename T>
class CBuffer final {

	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	ComPtr<ID3D12Resource> resources_;
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbView_{};

	T *mapData_;


public:

	CBuffer();
	CBuffer(const CBuffer &);
	~CBuffer() = default;

private:
};

template<typename T>
inline CBuffer<T>::CBuffer() {
	HRESULT result = S_FALSE;
	// 256バイト単位のアライメント
	resources_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), (sizeof(T) + 0xff) & ~0xff);

	cbView_.BufferLocation = resources_->GetGPUVirtualAddress();
	cbView_.SizeInBytes = static_cast<uint32_t>(resources_->GetDesc().Width);

	result = resources_->Map(0, nullptr, reinterpret_cast<void **>(&mapData_));
	assert(SUCCEEDED(result));
}

template<typename T>
inline CBuffer<T>::CBuffer(const CBuffer &other) {
	HRESULT result = S_FALSE;
	// 256バイト単位のアライメント
	resources_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), (sizeof(T) + 0xff) & ~0xff);

	cbView_.BufferLocation = resources_->GetGPUVirtualAddress();
	cbView_.SizeInBytes = static_cast<uint32_t>(resources_->GetDesc().Width);

	result = resources_->Map(0, nullptr, reinterpret_cast<void **>(&mapData_));
	assert(SUCCEEDED(result));

	// データのコピー
	*mapData_ = *other.mapData_;
}