#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <array>

#include "../Create/Create.h"
#include "DirectXCommon.h"

#pragma region 配列版の定数バッファ

/// @brief 配列版の定数バッファ
/// @tparam T 型名
/// @tparam U 配列の長さ
template<typename T, size_t U = 1u>
class CBuffer final {
	// 静的警告
	static_assert(!std::is_pointer<T>::value, "CBufferに与えた型がポインタ型です");
	static_assert(U > 0u, "CBufferに設定された数値が1以上ではありません");
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	ComPtr<ID3D12Resource> resources_ = nullptr;
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbView_{};

	T *mapData_;
public:

	const uint32_t size_ = U;

public:

	inline operator bool() const noexcept;		// 値が存在するか

	inline CBuffer &operator=(const T &other);	// コピー演算子

	size_t size()const noexcept { return U; }

	T *const begin() noexcept { return mapData_; }
	T *const end() noexcept { return mapData_[U]; }

	T &operator[](size_t index) noexcept { return mapData_[index]; }
	const T &operator[](size_t index) const noexcept { return mapData_[index]; }

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

template<typename T, size_t U>
inline CBuffer<T, U>::operator bool() const noexcept {
	return resources_ != nullptr;
}


template<typename T, size_t U>
inline CBuffer<T, U> &CBuffer<T, U>::operator=(const T &other) {
	*mapData_ = other;
	return *this;
}

template<typename T, size_t U>
inline CBuffer<T, U>::CBuffer() :size_(U) {
	CreateBuffer();
}

template<typename T, size_t U>
inline CBuffer<T, U>::CBuffer(const CBuffer &other) {
	CreateBuffer();

	// データのコピー
	*mapData_ = *other.mapData_;
}


template<typename T, size_t U>
inline void CBuffer<T, U>::CreateBuffer() {
	HRESULT result = S_FALSE;
	// 256バイト単位のアライメント
	resources_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), ((sizeof(T) * U) + 0xff) & ~0xff);

	cbView_.BufferLocation = resources_->GetGPUVirtualAddress();
	cbView_.SizeInBytes = static_cast<uint32_t>(resources_->GetDesc().Width);

	result = resources_->Map(0, nullptr, reinterpret_cast<void **>(&mapData_));
	assert(SUCCEEDED(result));

}

template<typename T, size_t U>
inline CBuffer<T, U>::~CBuffer() {
	resources_->Release();
	//cbView_ = {};
	//mapData_ = nullptr;
}

#pragma endregion

#pragma region 単体の場合に特殊化した定数バッファ

/// @brief 単体版定数バッファ
/// @tparam T 型名
template<typename T>
class CBuffer<T, 1u> final {
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
	//resources_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), (sizeof(T) + 0xff) & ~0xff);
	resources_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(T));

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

#pragma endregion
