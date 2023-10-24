#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <array>
#include <algorithm>

#include "../Create/Create.h"
#include "DirectXCommon.h"

#include "../../../Utils/SoLib/SoLib_Traits.h"

#pragma region 配列の定数バッファ

/// @brief 定数バッファ
/// @tparam T 型名 
template<SoLib::IsNotPointer T>
class ArrayBuffer final {
	// 静的警告
	static_assert(!std::is_pointer<T>::value, "ArrayBufferに与えた型がポインタ型です");
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	ComPtr<ID3D12Resource> resources_ = nullptr;
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc_{};

	uint32_t size_;

	T *mapData_;

public:

	inline ID3D12Resource *const GetResources() noexcept { return resources_.Get(); }
	inline const ID3D12Resource *const GetResources() const noexcept { return resources_.Get(); }

	inline const D3D12_SHADER_RESOURCE_VIEW_DESC &GetDesc() const noexcept { return srvDesc_; }

	inline operator bool() const noexcept;		// 値が存在するか

	inline operator T *() noexcept;				// 参照
	inline operator const T *() const noexcept;	// const参照

	inline T &operator[](uint32_t index) noexcept { return mapData_[index]; }
	inline const T &operator[](uint32_t index) const noexcept { return mapData_[index]; }

	inline T *const operator->() noexcept;					// dataのメンバへのアクセス
	inline const T *const operator->() const noexcept;		// dataのメンバへのアクセス(const)

	uint32_t size() const noexcept { return size_; }
	T *const begin() const noexcept { return &mapData_[0]; }
	T *const end() const noexcept { return &mapData_[size_]; }


	template <typename U>
	inline ArrayBuffer &operator=(const U &other);	// コピー演算子

public:
	inline D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const noexcept {
		return resources_->GetGPUVirtualAddress();
	}


public:

	ArrayBuffer(const uint32_t width = 0u);	// デフォルトコンストラクタ

	template <typename U>
	ArrayBuffer(const U &source);		// コピーコンストラクタ

	~ArrayBuffer();

	/// @brief バッファの計算
	void CreateBuffer(uint32_t size);


	void Copy(T *const begin, T *const end) {
		std::copy(begin, end, mapData_);
	}

private:
	/// @brief SrvDescを作成する
	D3D12_SHADER_RESOURCE_VIEW_DESC CreateSrvDesc() const;

};

template<SoLib::IsNotPointer T>
inline ArrayBuffer<T>::operator bool() const noexcept {
	return resources_ != nullptr;
}

template<SoLib::IsNotPointer T>
inline ArrayBuffer<T>::operator T *() noexcept {
	return *mapData_;
}

template<SoLib::IsNotPointer T>
inline ArrayBuffer<T>::operator const T *() const noexcept {
	return *mapData_;
}

template<SoLib::IsNotPointer T>
inline T *const ArrayBuffer<T>::operator->() noexcept {
	return *mapData_;
}

template<SoLib::IsNotPointer T>
inline const T *const ArrayBuffer<T>::operator->() const noexcept {
	return *mapData_;
}

template<SoLib::IsNotPointer T>
template<typename U>
inline ArrayBuffer<T> &ArrayBuffer<T>::operator=(const U &source) {
	static_assert(requires { source.size(); }, "与えられた型にsize()メンバ関数がありません");
	static_assert(requires { source.begin(); }, "与えられた型にbegin()メンバ関数がありません");
	static_assert(requires { source.end(); }, "与えられた型にend()メンバ関数がありません");

	CreateBuffer(static_cast<uint32_t>(source.size()));
	//Copy(source.begin(), source.end());
	std::copy(source.begin(), source.end(), mapData_);
	return *this;
}

template<SoLib::IsNotPointer T>
inline ArrayBuffer<T>::ArrayBuffer(const uint32_t width) {
	CreateBuffer(width);
}

template<SoLib::IsNotPointer T>
template <typename U>
inline ArrayBuffer<T>::ArrayBuffer(const U &source) {
	static_assert(requires { source.size(); }, "与えられた型にsize()メンバ関数がありません");
	static_assert(requires { source.begin(); }, "与えられた型にbegin()メンバ関数がありません");
	static_assert(requires { source.end(); }, "与えられた型にend()メンバ関数がありません");

	CreateBuffer(static_cast<uint32_t>(source.size()));
	std::copy(source.begin(), source.end(), mapData_);
}

//template<SoLib::IsNotPointer T>
//inline CBuffer<T>::CBuffer(CBuffer &&other) {
//	resources_ = other.resources_;
//	cbView_ = other.cbView_;
//	*mapData_ = *other.mapData_;
//
//}


template<SoLib::IsNotPointer T>
inline void ArrayBuffer<T>::CreateBuffer(uint32_t size) {
	// sizeが0以外である場合 && 現在の領域と異なる場合、領域を確保
	if (size != 0u && size_ != size) {
		size_ = size;
		HRESULT result = S_FALSE;
		if (resources_ != nullptr) { resources_->Release(); }
		// 256バイト単位のアライメント
		resources_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), (sizeof(T) * size + 0xff) & ~0xff);

		srvDesc_ = CreateSrvDesc();

		result = resources_->Map(0, nullptr, reinterpret_cast<void **>(&mapData_));
		assert(SUCCEEDED(result));
	}
}

template<SoLib::IsNotPointer T>
inline D3D12_SHADER_RESOURCE_VIEW_DESC ArrayBuffer<T>::CreateSrvDesc() const
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;

	srvDesc.Format = DXGI_FORMAT_UNKNOWN;	// 構造体の形は不明であるため
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;	// textureではなくbufferとして使うため
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	srvDesc.Buffer.StructureByteStride = sizeof(T);	// アライメントはC++準拠
	srvDesc.Buffer.NumElements = size_;

	return srvDesc;
}

template<SoLib::IsNotPointer T>
inline ArrayBuffer<T>::~ArrayBuffer() {
	resources_->Release();
}


#pragma endregion
