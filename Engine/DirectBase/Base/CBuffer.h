#pragma once
#include <wrl.h>
#include <d3d12.h>

#include "../Create/Create.h"
#include "DirectXCommon.h"

template<typename T>
class CBuffer final {
	// 静的警告
	static_assert(!std::is_pointer<T>::value, "CBufferに与えた型がポインタ型です");
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	ComPtr<ID3D12Resource> resources_ = nullptr;
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbView_{};

	T *mapData_;

	inline operator bool() noexcept;


public:

	CBuffer();					// デフォルトコンストラクタ
	CBuffer(const CBuffer &);	// コピーコンストラクタ
	//CBuffer(CBuffer &&);		// ムーブコンストラクタ

	~CBuffer();

private:

	void CreateBuffer();
};

template<typename T>
inline CBuffer<T>::operator bool() noexcept {
	return resources_ != nullptr;
}

template<typename T>
inline CBuffer<T>::CBuffer() {
	CreateBuffer();
}

template<typename T>
inline CBuffer<T>::CBuffer(const CBuffer &other) {
	CreateBuffer();

	// データのコピー
	*mapData_ = *other.mapData_;
}

//template<typename T>
//inline CBuffer<T>::CBuffer(CBuffer &&other) {
//	resources_ = other.resources_;
//	cbView_ = other.cbView_;
//	*mapData_ = *other.mapData_;
//
//}


template<typename T>
inline void CBuffer<T>::CreateBuffer() {
	HRESULT result = S_FALSE;
	// 256バイト単位のアライメント
	resources_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), (sizeof(T) + 0xff) & ~0xff);

	cbView_.BufferLocation = resources_->GetGPUVirtualAddress();
	cbView_.SizeInBytes = static_cast<uint32_t>(resources_->GetDesc().Width);

	result = resources_->Map(0, nullptr, reinterpret_cast<void **>(&mapData_));
	assert(SUCCEEDED(result));

}

template<typename T>
inline CBuffer<T>::~CBuffer() {
	resources_->Release();
	cbView_ = {};
	mapData_ = nullptr;
}
