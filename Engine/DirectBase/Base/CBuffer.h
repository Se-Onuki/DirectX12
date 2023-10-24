#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <array>
#include <algorithm>

#include "../Create/Create.h"
#include "DirectXCommon.h"

#include "../../../Utils/SoLib/SoLib_Traits.h"

#pragma region 単体定数バッファ

/// @brief 定数バッファ
/// @tparam T 型名
template<SoLib::IsNotPointer T, bool IsActive = true>
class CBuffer final {
	// 静的警告
	static_assert(!std::is_pointer<T>::value, "CBufferに与えた型がポインタ型です");
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	ComPtr<ID3D12Resource> resources_ = nullptr;
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbView_{};

	T *mapData_;

public:

	inline ID3D12Resource *const GetResources() noexcept { return &resources_.Get(); }
	inline const ID3D12Resource *const GetResources() const noexcept { return &resources_.Get(); }

	inline const D3D12_CONSTANT_BUFFER_VIEW_DESC &GetView() const noexcept { return cbView_; }

	inline operator bool() const noexcept;		// 値が存在するか

	inline operator T &() noexcept;				// 参照
	inline operator const T &() const noexcept;	// const参照

	inline T *const operator->() noexcept;					// dataのメンバへのアクセス
	inline T *const operator->() const noexcept;		// dataのメンバへのアクセス(const)


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

template<SoLib::IsNotPointer T, bool IsActive>
inline CBuffer<T, IsActive>::operator bool() const noexcept {
	return resources_ != nullptr;
}

template<SoLib::IsNotPointer T, bool IsActive>
inline CBuffer<T, IsActive>::operator T &() noexcept {
	return *mapData_;
}

template<SoLib::IsNotPointer T, bool IsActive>
inline CBuffer<T, IsActive>::operator const T &() const noexcept {
	return *mapData_;
}

template<SoLib::IsNotPointer T, bool IsActive>
inline T *const CBuffer<T, IsActive>::operator->() noexcept {
	return mapData_;
}

template<SoLib::IsNotPointer T, bool IsActive>
inline T *const CBuffer<T, IsActive>::operator->() const noexcept {
	return mapData_;
}

template<SoLib::IsNotPointer T, bool IsActive>
inline CBuffer<T, IsActive> &CBuffer<T, IsActive>::operator=(const T &other) {
	*mapData_ = static_cast<T>(other);
	return *this;
}

template<SoLib::IsNotPointer T, bool IsActive>
inline CBuffer<T, IsActive>::CBuffer() {
	CreateBuffer();
}

template<SoLib::IsNotPointer T, bool IsActive>
inline CBuffer<T, IsActive>::CBuffer(const CBuffer<T, IsActive> &other) {
	CreateBuffer();

	// データのコピー
	*mapData_ = *other.mapData_;
}

template<SoLib::IsNotPointer T, bool IsActive>
inline void CBuffer<T, IsActive>::CreateBuffer() {
	HRESULT result = S_FALSE;


	// 256バイト単位のアライメント
	resources_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), (sizeof(T) + 0xff) & ~0xff);


	cbView_.BufferLocation = resources_->GetGPUVirtualAddress();
	cbView_.SizeInBytes = static_cast<uint32_t>(resources_->GetDesc().Width);

	result = resources_->Map(0, nullptr, reinterpret_cast<void **>(&mapData_));
	assert(SUCCEEDED(result));

}

template<SoLib::IsNotPointer T, bool IsActive>
inline CBuffer<T, IsActive>::~CBuffer() {
	resources_->Release();
}

#pragma endregion

#pragma region バッファを持たないクラス

// バッファ機能を持たないクラス
template<SoLib::IsNotPointer T>
class CBuffer<T, false> {
public:
	CBuffer() = default;		// デフォルトコンストラクタ
	CBuffer(const CBuffer &) = default;	// コピーコンストラクタ
	CBuffer &operator=(const CBuffer &) = default;	// コピー演算子

	~CBuffer() = default;

	inline operator bool() const noexcept;		// 値が存在するか

	inline operator T &() noexcept;				// 参照
	inline operator const T &() const noexcept;	// const参照

	inline T *const operator->() noexcept;					// dataのメンバへのアクセス
	inline T *const operator->() const noexcept;		// dataのメンバへのアクセス(const)

	inline CBuffer &operator=(const T &other);	// コピー演算子

	void SetMapAddress(T *const ptr) { mapData_ = ptr; }

private:
	T *mapData_;
};

template<SoLib::IsNotPointer T>
inline CBuffer<T, false>::operator bool() const noexcept {
	return mapData_ != nullptr;
}

template<SoLib::IsNotPointer T>
inline CBuffer<T, false>::operator T &() noexcept {
	return *mapData_;
}

template<SoLib::IsNotPointer T>
inline CBuffer<T, false>::operator const T &() const noexcept {
	return *mapData_;
}

template<SoLib::IsNotPointer T>
inline T *const CBuffer<T, false>::operator->() noexcept {
	return mapData_;
}

template<SoLib::IsNotPointer T>
inline T *const CBuffer<T, false>::operator->() const noexcept {
	return mapData_;
}

template<SoLib::IsNotPointer T>
inline CBuffer<T, false> &CBuffer<T, false>::operator=(const T &other) {
	if (mapData_) {
		*mapData_ = static_cast<T>(other);
		return *this;
	}
}

#pragma endregion