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
	*mapData_ = other;
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

// バッファ機能を持たないクラス
template<SoLib::IsNotPointer T>
class CBuffer<T, false> {};

#pragma region 配列の定数バッファ

/// @brief 定数バッファ
/// @tparam T 型名 
template<SoLib::IsNotPointer T>
class StructuredBuffer final {
	// 静的警告
	static_assert(!std::is_pointer<T>::value, "CBufferに与えた型がポインタ型です");
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
	inline StructuredBuffer &operator=(const U &other);	// コピー演算子

public:
	inline D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const noexcept {
		return resources_->GetGPUVirtualAddress();
	}


public:

	StructuredBuffer(const uint32_t width = 0u);	// デフォルトコンストラクタ

	template <typename U>
	StructuredBuffer(const U &source);		// コピーコンストラクタ

	~StructuredBuffer();

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
inline StructuredBuffer<T>::operator bool() const noexcept {
	return resources_ != nullptr;
}

template<SoLib::IsNotPointer T>
inline StructuredBuffer<T>::operator T *() noexcept {
	return *mapData_;
}

template<SoLib::IsNotPointer T>
inline StructuredBuffer<T>::operator const T *() const noexcept {
	return *mapData_;
}

template<SoLib::IsNotPointer T>
inline T *const StructuredBuffer<T>::operator->() noexcept {
	return *mapData_;
}

template<SoLib::IsNotPointer T>
inline const T *const StructuredBuffer<T>::operator->() const noexcept {
	return *mapData_;
}

template<SoLib::IsNotPointer T>
template<typename U>
inline StructuredBuffer<T> &StructuredBuffer<T>::operator=(const U &source) {
	static_assert(requires { source.size(); }, "与えられた型にsize()メンバ関数がありません");
	static_assert(requires { source.begin(); }, "与えられた型にbegin()メンバ関数がありません");
	static_assert(requires { source.end(); }, "与えられた型にend()メンバ関数がありません");

	CreateBuffer(static_cast<uint32_t>(source.size()));
	//Copy(source.begin(), source.end());
	std::copy(source.begin(), source.end(), mapData_);
	return *this;
}

template<SoLib::IsNotPointer T>
inline StructuredBuffer<T>::StructuredBuffer(const uint32_t width) {
	CreateBuffer(width);
}

template<SoLib::IsNotPointer T>
template <typename U>
inline StructuredBuffer<T>::StructuredBuffer(const U &source) {
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
inline void StructuredBuffer<T>::CreateBuffer(uint32_t size) {
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
inline D3D12_SHADER_RESOURCE_VIEW_DESC StructuredBuffer<T>::CreateSrvDesc() const
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
inline StructuredBuffer<T>::~StructuredBuffer() {
	resources_->Release();
}


#pragma endregion



#pragma region 頂点バッファ

/// @brief 頂点バッファ
/// @tparam T 頂点データの型 Index 添え字が有効か
template <SoLib::IsNotPointer T, bool Index = true>
class VertexCBuffer final {

	StructuredBuffer<T> vertexData_;
	D3D12_VERTEX_BUFFER_VIEW vbView_;

	StructuredBuffer<uint32_t> indexData_;
	D3D12_INDEX_BUFFER_VIEW ibView_;

public:
	VertexCBuffer() = default;
	VertexCBuffer(const VertexCBuffer &) = default;
	~VertexCBuffer() = default;

	auto &GetVertexData()  noexcept { return vertexData_; }
	const auto &GetVertexData() const noexcept { return vertexData_; }
	const auto &GetVBView() const noexcept { return vbView_; };

	auto &GetIndexData()  noexcept { return indexData_; }
	const auto &GetIndexData() const noexcept { return indexData_; }
	const auto &GetIBView() const noexcept { return ibView_; };

	template <typename U>
	void SetVertexData(const U &source);
	template <typename U>
	void SetIndexData(const U &source);
};

template <SoLib::IsNotPointer T, bool Index>
template <typename U>
void VertexCBuffer<T, Index>::SetVertexData(const U &source) {
	static_assert(requires { source.size(); }, "与えられた型にsize()メンバ関数がありません");
	static_assert(requires { source.begin(); }, "与えられた型にbegin()メンバ関数がありません");
	static_assert(requires { source.end(); }, "与えられた型にend()メンバ関数がありません");

	vertexData_ = source;

	// 頂点バッファビューを作成する
	// リソースの先頭のアドレスから使う
	vbView_.BufferLocation = vertexData_.GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点3つ分のサイズ
	vbView_.SizeInBytes = static_cast<UINT>(sizeof(T) * vertexData_.size());
	// 1頂点あたりのサイズ
	vbView_.StrideInBytes = sizeof(T);

}

template <SoLib::IsNotPointer T, bool Index>
template <typename U>
void VertexCBuffer<T, Index>::SetIndexData(const U &source) {
	static_assert(requires { source.size(); }, "与えられた型にsize()メンバ関数がありません");
	static_assert(requires { source.begin(); }, "与えられた型にbegin()メンバ関数がありません");
	static_assert(requires { source.end(); }, "与えられた型にend()メンバ関数がありません");

	indexData_ = source;

	// インデックスview
	ibView_.BufferLocation = indexData_.GetGPUVirtualAddress();
	ibView_.SizeInBytes = static_cast<UINT>(sizeof(uint32_t) * indexData_.size());
	ibView_.Format = DXGI_FORMAT_R32_UINT;
}

#pragma endregion
