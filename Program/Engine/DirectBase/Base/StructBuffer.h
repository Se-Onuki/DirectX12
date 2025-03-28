/// @file StructBuffer.h
/// @brief ストラクチャードバッファの実装
/// @author ONUKI seiya
#pragma once
#include <algorithm>
#include <array>
#include <d3d12.h>
#include <wrl.h>

#include "../Create/Create.h"
#include "../DxResource/DxResourceBuffer.h"
#include "../DxResource/DxResourceBufferPoolManager.h"
#include "CBuffer.h"
#include "DirectXCommon.h"

#include "../../Engine/Utils/SoLib/SoLib_Traits.h"

#pragma region 配列の定数バッファ

/// @class ArrayBuffer
/// @brief 配列バッファ
/// @tparam T 型名
template <SoLib::IsRealType T, D3D12_HEAP_TYPE HeapType = D3D12_HEAP_TYPE_UPLOAD>
class ArrayBuffer final
{
	// 静的警告
	static_assert(!std::is_pointer<T>::value, "ArrayBufferに与えた型がポインタ型です");
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	using ResourceManager = SolEngine::DxResourceBufferPoolManager<HeapType>;
	using ResourceHandle = ResourceManager::UniqueHandle;

	ResourceHandle resourceHandle_;

	uint32_t size_;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc_{};

public:
	using map_matrix = T;

	/// @brief リソースの取得
	/// @return リソース
	inline ID3D12Resource *const GetResources() noexcept { return resourceHandle_->GetResource(); }
	inline ID3D12Resource *const GetResources() const noexcept { return resourceHandle_->GetResource(); }

	/// @brief SRVDescの取得
	/// @return SRVDesc
	inline const D3D12_SHADER_RESOURCE_VIEW_DESC &GetDesc() const noexcept { return srvDesc_; }

	inline operator bool() const noexcept; // 値が存在するか

	inline operator T *() noexcept;				// 参照
	inline operator const T *() const noexcept; // const参照

	/// @brief 添え字演算子
	/// @param[in] index 添え字
	inline T &operator[](uint32_t index) noexcept
	{
		return resourceHandle_->GetAccessor<T>()[index];
	}
	inline const T &operator[](uint32_t index) const noexcept
	{
		return resourceHandle_->GetAccessor<T>()[index];
	}

	inline T *const operator->() noexcept;			   // dataのメンバへのアクセス
	inline const T *const operator->() const noexcept; // dataのメンバへのアクセス(const)

	/// @brief 配列のサイズ
	/// @return 配列のサイズ
	uint32_t size() const noexcept { return size_; }
	/// @brief 先頭アドレスの取得
	/// @return 先頭アドレス
	T *const data() const noexcept { return resourceHandle_->GetAccessor<T>().data(); }
	/// @brief 開始イテレータの取得
	/// @return 開始イテレータ
	auto begin() const noexcept { return resourceHandle_->GetAccessor<T>().begin(); }
	/// @brief 番兵の取得
	/// @return 番兵
	auto end() const noexcept { return begin() + size(); }

	template <SoLib::IsContainsType<T> U>
	inline ArrayBuffer &operator=(const U &other); // コピー演算子

public:
	/// @brief GPU仮想アドレスの取得
	inline D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const noexcept
	{
		return resourceHandle_->GetCBView().BufferLocation;
	}

public:
	ArrayBuffer(const uint32_t width = 0u); // デフォルトコンストラクタ

	template <SoLib::IsContainsType<T> U>
	ArrayBuffer(const U &source); // コピーコンストラクタ

	~ArrayBuffer();

	/// @brief バッファの計算
	void CreateBuffer(uint32_t buffSize);

	/// @brief コピー
	/// @param[in] begin コピー開始アドレス
	/// @param[in] end 番兵アドレス
	void Copy(T *const begin, T *const end)
		requires(ResourceManager::kHasMemory_)
	{
		std::copy(begin, end, resourceHandle_->begin());
	}

private:
	/// @brief SrvDescを作成する
	D3D12_SHADER_RESOURCE_VIEW_DESC CreateSrvDesc() const;
};

template <SoLib::IsRealType T, D3D12_HEAP_TYPE HeapType>
inline ArrayBuffer<T, HeapType>::operator bool() const noexcept
{
	return static_cast<bool>(resourceHandle_);
}

template <SoLib::IsRealType T, D3D12_HEAP_TYPE HeapType>
inline ArrayBuffer<T, HeapType>::operator T *() noexcept
{
	return resourceHandle_->GetAccessor<T>().data();
}

template <SoLib::IsRealType T, D3D12_HEAP_TYPE HeapType>
inline ArrayBuffer<T, HeapType>::operator const T *() const noexcept
{
	return resourceHandle_->GetAccessor<T>().data();
}

template <SoLib::IsRealType T, D3D12_HEAP_TYPE HeapType>
inline T *const ArrayBuffer<T, HeapType>::operator->() noexcept
{
	return resourceHandle_->GetAccessor<T>().data();
}

template <SoLib::IsRealType T, D3D12_HEAP_TYPE HeapType>
inline const T *const ArrayBuffer<T, HeapType>::operator->() const noexcept
{
	return resourceHandle_->GetAccessor<T>().data();
}

template <SoLib::IsRealType T, D3D12_HEAP_TYPE HeapType>
template <SoLib::IsContainsType<T> U>
inline ArrayBuffer<T, HeapType> &ArrayBuffer<T, HeapType>::operator=(const U &source)
{
	CreateBuffer(static_cast<uint32_t>(source.size()));
	std::copy(source.begin(), source.end(), begin());
	return *this;
}

template <SoLib::IsRealType T, D3D12_HEAP_TYPE HeapType>
inline ArrayBuffer<T, HeapType>::ArrayBuffer(const uint32_t width)
{
	CreateBuffer(width);
}

template <SoLib::IsRealType T, D3D12_HEAP_TYPE HeapType>
template <SoLib::IsContainsType<T> U>
inline ArrayBuffer<T, HeapType>::ArrayBuffer(const U &source)
{
	static_assert(requires { source.size(); }, "与えられた型にsize()メンバ関数がありません");
	static_assert(requires { source.begin(); }, "与えられた型にbegin()メンバ関数がありません");
	static_assert(requires { source.end(); }, "与えられた型にend()メンバ関数がありません");

	CreateBuffer(static_cast<uint32_t>(source.size()));
	std::copy(source.begin(), source.end(), begin());
}

template <SoLib::IsRealType T, D3D12_HEAP_TYPE HeapType>
inline void ArrayBuffer<T, HeapType>::CreateBuffer(uint32_t buffSize)
{
	// sizeが0以外である場合 && 現在の領域と異なる場合、領域を確保
	if (buffSize != 0u && size() != buffSize) {
		// HRESULT result = S_FALSE;
		ResourceManager *const resourceManager = ResourceManager::GetInstance();
		// もしすでに領域が確保されていたら解放
		if (resourceHandle_) {
			resourceHandle_ = {};
		}
		// 256バイト単位のアライメント

		resourceHandle_ = resourceManager->PushBack<T>(buffSize);

		size_ = buffSize;

		srvDesc_ = CreateSrvDesc();
	}
}

template <SoLib::IsRealType T, D3D12_HEAP_TYPE HeapType>
inline D3D12_SHADER_RESOURCE_VIEW_DESC ArrayBuffer<T, HeapType>::CreateSrvDesc() const
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;

	srvDesc.Format = DXGI_FORMAT_UNKNOWN; // 構造体の形は不明であるため
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER; // textureではなくbufferとして使うため
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	srvDesc.Buffer.StructureByteStride = sizeof(T); // アライメントはC++準拠
	srvDesc.Buffer.NumElements = size();

	return srvDesc;
}

template <SoLib::IsRealType T, D3D12_HEAP_TYPE HeapType>
inline ArrayBuffer<T, HeapType>::~ArrayBuffer()
{
	// if (resources_ != nullptr) { resources_->Release(); }
}

#pragma endregion


/// @brief 構造化バッファ
/// @tparam T 型名
template <SoLib::IsRealType T>
class StructuredBuffer
{
public:
	/// @brief コンストラクタ
	/// @param[in] maxSize 最大サイズ 
	StructuredBuffer(uint32_t maxSize = 0u);
	StructuredBuffer &operator=(const StructuredBuffer &) = default;
	~StructuredBuffer() = default;

public:
	/// @brief バッファの取得
	/// @return バッファ
	inline const ArrayBuffer<T> &GetBuffer() const { return arrayBuffer_; }

	/// @brief リソースの取得
	/// @return リソース
	inline ID3D12Resource *const GetResources() noexcept { return arrayBuffer_.GetResources(); }
	inline ID3D12Resource *const GetResources() const noexcept { return arrayBuffer_.GetResources(); }

	/// @brief SRVDescの取得
	/// @return SRVDesc
	inline const D3D12_SHADER_RESOURCE_VIEW_DESC &GetDesc() const noexcept { return arrayBuffer_.GetDesc(); }

	inline operator bool() const noexcept { return static_cast<bool>(arrayBuffer_); } // 値が存在するか

	inline operator T *() noexcept { return arrayBuffer_.data(); }			   // 参照
	inline operator const T *() const noexcept { return arrayBuffer_.data(); } // const参照

	inline T &operator[](uint32_t index) noexcept { return arrayBuffer_[index]; }
	inline const T &operator[](uint32_t index) const noexcept { return arrayBuffer_[index]; }

	inline T *const operator->() noexcept { return arrayBuffer_.data(); }			  // dataのメンバへのアクセス
	inline const T *const operator->() const noexcept { return arrayBuffer_.data(); } // dataのメンバへのアクセス(const)

	/// @brief 保存されている要素数
	/// @return 要素数
	uint32_t size() const noexcept { return size_; }

	/// @brief 先頭アドレスの取得
	/// @return 先頭アドレス
	T *const data() noexcept { return arrayBuffer_.data(); }
	const T *const data() const noexcept { return arrayBuffer_.data(); }

	/// @brief 先頭イテレータの取得
	/// @return 先頭イテレータ
	T *const begin() noexcept { return &arrayBuffer_.begin(); }
	const T *const begin() const noexcept { return &arrayBuffer_.begin(); }
	/// @brief 先頭イテレータの取得
	/// @return 先頭イテレータ
	const T *const cbegin() const noexcept { return &arrayBuffer_.begin(); }

	/// @brief 番兵の取得
	/// @return 番兵
	T *const end() noexcept { return &arrayBuffer_.end(); }
	const T *const end() const noexcept { return &arrayBuffer_.end(); }
	/// @brief 番兵の取得
	/// @return 番兵
	const T *const cend() const noexcept { return &arrayBuffer_.end(); }

	/// @brief 追加
	/// @param[in] data 追加するデータ
	void push_back(const T &data);
	void push_back(T &&data);

	/// @brief 書き込み可能メモリを渡す
	/// @param[in] count 確保量
	/// @return 書き込み先のデータ
	std::span<T> Reservation(size_t count)
	{
		// 書き込み先の配列データ
		std::span<T> result;
		// 範囲外なら
		if (size_ >= arrayBuffer_.size()) {
			// そのまま帰す
			return result;
		}

		// 確保したあとの長さ
		size_t afterSize = size_ + count;
		// 範囲外に行っていたら
		if (arrayBuffer_.size() < afterSize) {
			// 長さと同じになるように調整
			afterSize = arrayBuffer_.size();
		}
		// 返す長さ
		size_t length = afterSize - size_;
		// メモリを渡す
		result = { &arrayBuffer_[size_], length };
		// 長さを上書きする
		size_ = static_cast<uint32_t>(afterSize);
		// 返す
		return result;
	}

	/// @brief 書き込み先の配列をクリアする
	void clear();

	template <SoLib::IsContainer U>
	inline StructuredBuffer &operator=(const U &other); // コピー演算子

	/// @brief ヒープの取得
	/// @return ヒープ
	const auto &GetHeapRange() const { return heapRange_; }

	/// @brief スタートインデックスの取得
	/// @return スタートインデックス
	const CBuffer<uint32_t> &GetStartIndex() const { return startIndex_; }

private:
	ArrayBuffer<T> arrayBuffer_;
	DescHeap<D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV>::HeapRange heapRange_;
	CBuffer<uint32_t> startIndex_;

	uint32_t size_{};
};

template <SoLib::IsRealType T>
inline StructuredBuffer<T>::StructuredBuffer(uint32_t maxSize)
{
	arrayBuffer_.CreateBuffer(maxSize);
	startIndex_ = 0u;

	static auto *const device = DirectXCommon::GetInstance()->GetDevice();
	static auto *const srvHeap = DirectXCommon::GetInstance()->GetSRVHeap();

	heapRange_ = srvHeap->RequestHeapAllocation(1u);
	device->CreateShaderResourceView(GetResources(), &GetDesc(), heapRange_.GetHandle(0u).cpuHandle_);
}

template <SoLib::IsRealType T>
inline void StructuredBuffer<T>::push_back(const T &data)
{
	// もし、最大値の方が大きかったら
	if (size_ < arrayBuffer_.size()) {
		// データを追加して、インクリメント
		arrayBuffer_[size_++] = data;
	}
}

template <SoLib::IsRealType T>
inline void StructuredBuffer<T>::push_back(T &&data)
{
	if (size_ < arrayBuffer_.size()) {
		arrayBuffer_[size_++] = std::move(data);
	}
}

template <SoLib::IsRealType T>
inline void StructuredBuffer<T>::clear()
{
	size_ = 0u;
}

template <SoLib::IsRealType T>
template <SoLib::IsContainer U>
inline StructuredBuffer<T> &StructuredBuffer<T>::operator=(const U &other)
{
	arrayBuffer_ = other;
}
