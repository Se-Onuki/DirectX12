#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <array>

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

public:

	inline operator bool() const noexcept;		// 値が存在するか

	inline operator T &() noexcept;				// 参照
	inline operator const T &() const noexcept;	// const参照

	inline T *const operator->() noexcept;					// dataのメンバへのアクセス
	inline const T *const operator->() const noexcept;		// dataのメンバへのアクセス(const)


	inline CBuffer &operator=(const T &other);	// コピー演算子

public:
	inline D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const noexcept {
		return resources_->GetGPUVirtualAddress();
	}


public:

	CBuffer();					// デフォルトコンストラクタ
	CBuffer(const CBuffer &);	// コピーコンストラクタ

	~CBuffer();

private:

	void CreateBuffer();
};

template<typename T>
inline CBuffer<T>::operator bool() const noexcept {
	return resources_ != nullptr;
}

template<typename T>
inline CBuffer<T>::operator T &() noexcept {
	return *mapData_;
}

template<typename T>
inline CBuffer<T>::operator const T &() const noexcept {
	return *mapData_;
}

template<typename T>
inline T *const CBuffer<T>::operator->() noexcept {
	return mapData_;
}

template<typename T>
inline const T *const CBuffer<T>::operator->() const noexcept {
	return mapData_;
}

template<typename T>
inline CBuffer<T> &CBuffer<T>::operator=(const T &other) {
	*mapData_ = other;
	return *this;
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
	//cbView_ = {};
	//mapData_ = nullptr;
}
