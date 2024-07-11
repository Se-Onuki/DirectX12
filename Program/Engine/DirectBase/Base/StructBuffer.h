#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <array>
#include <algorithm>

#include "../Create/Create.h"
#include "DirectXCommon.h"
#include "CBuffer.h"
#include "../DxResource/DxResourceBuffer.h"
#include "../DxResource/DxResourceBufferPoolManager.h"

#include "../../../Utils/SoLib/SoLib_Traits.h"

#pragma region 配列の定数バッファ

/// @brief 定数バッファ
/// @tparam T 型名 
template<SoLib::IsRealType T, D3D12_HEAP_TYPE HeapType = D3D12_HEAP_TYPE_UPLOAD>
class ArrayBuffer final {
	// 静的警告
	static_assert(!std::is_pointer<T>::value, "ArrayBufferに与えた型がポインタ型です");
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	using ResourceManager = SolEngine::DxResourceBufferPoolManager<HeapType>;
	using ResourceHandle = ResourceManager::UniqueHandle;

	ResourceHandle resourceHandle_;

	uint32_t size_;

	//ComPtr<ID3D12Resource> resources_ = nullptr;
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc_{};

	//std::span<T> mapData_ = {};

public:

	using map_matrix = T;

	inline ID3D12Resource *const GetResources() noexcept { return resourceHandle_->GetResource(); }
	inline ID3D12Resource *const GetResources() const noexcept { return resourceHandle_->GetResource(); }

	inline const D3D12_SHADER_RESOURCE_VIEW_DESC &GetDesc() const noexcept { return srvDesc_; }

	inline operator bool() const noexcept;		// 値が存在するか

	inline operator T *() noexcept;				// 参照
	inline operator const T *() const noexcept;	// const参照

	inline T &operator[](uint32_t index) noexcept { 
		return resourceHandle_->GetAccessor<T>()[index]; }
	inline const T &operator[](uint32_t index) const noexcept { 
		return resourceHandle_->GetAccessor<T>()[index]; }

	inline T *const operator->() noexcept;					// dataのメンバへのアクセス
	inline const T *const operator->() const noexcept;		// dataのメンバへのアクセス(const)

	uint32_t size() const noexcept { return size_; }
	T *const data() const noexcept { return resourceHandle_->GetAccessor<T>().data(); }
	auto begin() const noexcept { return resourceHandle_->GetAccessor<T>().begin(); }
	auto end() const noexcept { return begin() + size(); }


	template <SoLib::IsContainsType<T> U>
	inline ArrayBuffer &operator=(const U &other);	// コピー演算子

public:
	inline D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const noexcept {
		return resourceHandle_->GetCBView().BufferLocation;
	}


public:

	ArrayBuffer(const uint32_t width = 0u);	// デフォルトコンストラクタ

	template<SoLib::IsContainsType<T> U>
	ArrayBuffer(const U &source);		// コピーコンストラクタ

	~ArrayBuffer();

	/// @brief バッファの計算
	void CreateBuffer(uint32_t buffSize);


	void Copy(T *const begin, T *const end) requires(ResourceManager::kHasMemory_) {
		std::copy(begin, end, resourceHandle_->begin());
	}

private:
	/// @brief SrvDescを作成する
	D3D12_SHADER_RESOURCE_VIEW_DESC CreateSrvDesc() const;

};

template<SoLib::IsRealType T, D3D12_HEAP_TYPE HeapType>
inline ArrayBuffer<T, HeapType>::operator bool() const noexcept {
	return static_cast<bool>(resourceHandle_);
}

template<SoLib::IsRealType T, D3D12_HEAP_TYPE HeapType>
inline ArrayBuffer<T, HeapType>::operator T *() noexcept {
	return resourceHandle_->GetAccessor<T>().data();
}

template<SoLib::IsRealType T, D3D12_HEAP_TYPE HeapType>
inline ArrayBuffer<T, HeapType>::operator const T *() const noexcept {
	return resourceHandle_->GetAccessor<T>().data();
}

template<SoLib::IsRealType T, D3D12_HEAP_TYPE HeapType>
inline T *const ArrayBuffer<T, HeapType>::operator->() noexcept {
	return resourceHandle_->GetAccessor<T>().data();
}

template<SoLib::IsRealType T, D3D12_HEAP_TYPE HeapType>
inline const T *const ArrayBuffer<T, HeapType>::operator->() const noexcept {
	return resourceHandle_->GetAccessor<T>().data();
}

template<SoLib::IsRealType T, D3D12_HEAP_TYPE HeapType>
template<SoLib::IsContainsType<T> U>
inline ArrayBuffer<T, HeapType> &ArrayBuffer<T, HeapType>::operator=(const U &source) {
	CreateBuffer(static_cast<uint32_t>(source.size()));
	std::copy(source.begin(), source.end(), begin());
	return *this;
}

template<SoLib::IsRealType T, D3D12_HEAP_TYPE HeapType>
inline ArrayBuffer<T, HeapType>::ArrayBuffer(const uint32_t width) {
	CreateBuffer(width);
}

template<SoLib::IsRealType T, D3D12_HEAP_TYPE HeapType>
template<SoLib::IsContainsType<T> U>
inline ArrayBuffer<T, HeapType>::ArrayBuffer(const U &source) {
	static_assert(requires { source.size(); }, "与えられた型にsize()メンバ関数がありません");
	static_assert(requires { source.begin(); }, "与えられた型にbegin()メンバ関数がありません");
	static_assert(requires { source.end(); }, "与えられた型にend()メンバ関数がありません");

	CreateBuffer(static_cast<uint32_t>(source.size()));
	std::copy(source.begin(), source.end(), begin());
}

//template<SoLib::IsRealType T>
//inline CBuffer<T>::CBuffer(CBuffer &&other) {
//	resources_ = other.resources_;
//	cbView_ = other.cbView_;
//	*mapData_ = *other.mapData_;
//
//}


template<SoLib::IsRealType T, D3D12_HEAP_TYPE HeapType>
inline void ArrayBuffer<T, HeapType>::CreateBuffer(uint32_t buffSize) {
	// sizeが0以外である場合 && 現在の領域と異なる場合、領域を確保
	if (buffSize != 0u && size() != buffSize) {
		//HRESULT result = S_FALSE;
		ResourceManager *const resourceManager = ResourceManager::GetInstance();
		if (resourceHandle_) { resourceHandle_ = {}; }
		// 256バイト単位のアライメント

		resourceHandle_ = resourceManager->PushBack<T>(buffSize);

		size_ = buffSize;

		srvDesc_ = CreateSrvDesc();

	}
}

template<SoLib::IsRealType T, D3D12_HEAP_TYPE HeapType>
inline D3D12_SHADER_RESOURCE_VIEW_DESC ArrayBuffer<T, HeapType>::CreateSrvDesc() const
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;

	srvDesc.Format = DXGI_FORMAT_UNKNOWN;	// 構造体の形は不明であるため
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;	// textureではなくbufferとして使うため
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	srvDesc.Buffer.StructureByteStride = sizeof(T);	// アライメントはC++準拠
	srvDesc.Buffer.NumElements = size();

	return srvDesc;
}

template<SoLib::IsRealType T, D3D12_HEAP_TYPE HeapType>
inline ArrayBuffer<T, HeapType>::~ArrayBuffer() {
	//if (resources_ != nullptr) { resources_->Release(); }
}


#pragma endregion

template<SoLib::IsRealType T>
class StructuredBuffer {
public:
	StructuredBuffer(uint32_t maxSize = 0u);
	StructuredBuffer &operator= (const StructuredBuffer &) = default;
	~StructuredBuffer() = default;
public:

	inline const ArrayBuffer<T> &GetBuffer() const { return arrayBuffer_; }

	inline ID3D12Resource *const GetResources() noexcept { return arrayBuffer_.GetResources(); }
	inline ID3D12Resource *const GetResources() const noexcept { return arrayBuffer_.GetResources(); }

	inline const D3D12_SHADER_RESOURCE_VIEW_DESC &GetDesc() const noexcept { return arrayBuffer_.GetDesc(); }

	inline operator bool() const noexcept { return static_cast<bool>(arrayBuffer_); }		// 値が存在するか

	inline operator T *() noexcept { return arrayBuffer_.data(); }			// 参照
	inline operator const T *() const noexcept { return arrayBuffer_.data(); }	// const参照

	inline T &operator[](uint32_t index) noexcept { return arrayBuffer_[index]; }
	inline const T &operator[](uint32_t index) const noexcept { return arrayBuffer_[index]; }

	inline T *const operator->() noexcept { return arrayBuffer_.data(); }					// dataのメンバへのアクセス
	inline const T *const operator->() const noexcept { return arrayBuffer_.data(); }	// dataのメンバへのアクセス(const)

	uint32_t size() const noexcept { return size_; }

	T *const data() noexcept { return arrayBuffer_.data(); }
	const T *const data() const noexcept { return arrayBuffer_.data(); }

	T *const begin() noexcept { return &arrayBuffer_.begin(); }
	const T *const begin() const noexcept { return &arrayBuffer_.begin(); }
	const T *const cbegin() const noexcept { return &arrayBuffer_.begin(); }

	T *const end() noexcept { return &arrayBuffer_.end(); }
	const T *const end() const noexcept { return &arrayBuffer_.end(); }
	const T *const cend() const noexcept { return &arrayBuffer_.end(); }

	void push_back(const T &data);
	void push_back(T &&data);

	void clear();

	template <SoLib::IsContainer U>
	inline StructuredBuffer &operator=(const U &other);	// コピー演算子

	const auto &GetHeapRange() const { return heapRange_; }

	const CBuffer<uint32_t> &GetStartIndex() const { return startIndex_; }


private:

	ArrayBuffer<T> arrayBuffer_;
	DescHeap<D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV>::HeapRange heapRange_;
	CBuffer<uint32_t> startIndex_;

	uint32_t size_;

};

template<SoLib::IsRealType T>
inline StructuredBuffer<T>::StructuredBuffer(uint32_t maxSize) {
	arrayBuffer_.CreateBuffer(maxSize);
	startIndex_ = 0u;

	static auto *const device = DirectXCommon::GetInstance()->GetDevice();
	static auto *const srvHeap = DirectXCommon::GetInstance()->GetSRVHeap();

	heapRange_ = srvHeap->RequestHeapAllocation(1u);
	device->CreateShaderResourceView(GetResources(), &GetDesc(), heapRange_.GetHandle(0u).cpuHandle_);
}

template<SoLib::IsRealType T>
inline void StructuredBuffer<T>::push_back(const T &data) {
	// もし、最大値の方が大きかったら
	if (size_ < arrayBuffer_.size()) {
		// データを追加して、インクリメント
		arrayBuffer_[size_++] = data;
	}
}

template<SoLib::IsRealType T>
inline void StructuredBuffer<T>::push_back(T &&data) {
	if (size_ < arrayBuffer_.size()) {
		arrayBuffer_[size_++] = std::move(data);
	}
}

template<SoLib::IsRealType T>
inline void StructuredBuffer<T>::clear() {
	size_ = 0u;
}

template<SoLib::IsRealType T>
template<SoLib::IsContainer U>
inline StructuredBuffer<T> &StructuredBuffer<T>::operator=(const U &other) {
	arrayBuffer_ = other;
}
