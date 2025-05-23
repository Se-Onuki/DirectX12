/// @file CBuffer.h
/// @brief コンスタントバッファの実装
/// @author ONUKI seiya
#pragma once
#include <algorithm>
#include <array>
#include <d3d12.h>
#include <wrl.h>

#include "../Create/Create.h"
#include "DirectXCommon.h"

#include "../../Engine/Utils/SoLib/SoLib_Traits.h"
#include "../String/String.h"

#include "../DxResource/DxResourceBufferPoolManager.h"

#pragma region 単体定数バッファ

/// @class CBuffer
/// @brief 定数バッファ
/// @tparam T 型名
/// @tparam IsActive バッファを持つか否か
template <SoLib::IsNotPointer T, bool IsActive = true>
class CBuffer final
{
	// 静的警告
	static_assert(!std::is_pointer<T>::value, "CBufferに与えた型がポインタ型です");
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	/// @brief リソースとして保持する構造体
	SolEngine::DxResourceBufferPoolManager<>::UniqueHandle buffer_;

public:
	/// @fn ID3D12Resource *const GetResources(void) noexcept
	/// @brief リソース取得
	/// @return リソースのポインタ
	inline ID3D12Resource *const GetResources() noexcept { return buffer_->GetResource(); }
	/// @fn const ID3D12Resource *const GetResources(void) const noexcept
	/// @brief リソース取得(const)
	/// @return リソースのポインタ
	inline const ID3D12Resource *const GetResources() const noexcept { return buffer_->GetResource(); }

	/// @fn const D3D12_CONSTANT_BUFFER_VIEW_DESC &GetView(void) const noexcept
	/// @brief コンスタントバッファビュー取得
	/// @return コンスタントバッファビュー
	inline const D3D12_CONSTANT_BUFFER_VIEW_DESC &GetView() const noexcept { return buffer_->GetCBView(); }

	inline operator bool() const noexcept; // 値が存在するか

	inline operator T &() noexcept;				// 参照
	inline operator const T &() const noexcept; // const参照

	inline T *const operator->() noexcept;		 // dataのメンバへのアクセス
	inline T *const operator->() const noexcept; // dataのメンバへのアクセス(const)

	inline CBuffer &operator=(const T &other); // コピー演算子

public:
	/// @fn D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress(void)
	/// @brief 仮想アドレス取得
	/// @return 仮想アドレス
	inline D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const noexcept
	{
		return buffer_->GetCBView().BufferLocation;
	}

	/// @fn T *const get(void)
	/// @brief データ取得
	/// @return データ
	/// @details T型のポインタを返す
	inline T *const get() { return buffer_->GetAccessor<T>().data(); }
	inline const T *const get() const { return buffer_->GetAccessor<T>().data(); }

	/// @fn void SetName(void)
	/// @brief デバッグで使用される名前を設定する
	template <typename U = CBuffer>
	void SetName();

public:
	CBuffer();				  // デフォルトコンストラクタ
	CBuffer(const CBuffer &); // コピーコンストラクタ
	CBuffer(CBuffer &&);	  // ムーブコンストラクタ

	CBuffer(const T &r);

	CBuffer &operator=(const CBuffer &other) { return *this = static_cast<const T &>(other); } // コピー演算子
	CBuffer &operator=(CBuffer &&);															   // ムーブ演算子

	~CBuffer();

private:
	/// @fn void CreateBuffer(void)
	/// @brief リソース作成
	void CreateBuffer();
};

template <SoLib::IsNotPointer T, bool IsActive>
inline CBuffer<T, IsActive>::operator bool() const noexcept
{
	return buffer_.GetResource() != nullptr;
}

template <SoLib::IsNotPointer T, bool IsActive>
inline CBuffer<T, IsActive>::operator T &() noexcept
{
	return *reinterpret_cast<T *>(buffer_->data());
}

template <SoLib::IsNotPointer T, bool IsActive>
inline CBuffer<T, IsActive>::operator const T &() const noexcept
{
	return *reinterpret_cast<const T *>(buffer_->data());
}

template <SoLib::IsNotPointer T, bool IsActive>
inline T *const CBuffer<T, IsActive>::operator->() noexcept
{
	return reinterpret_cast<T *>(buffer_->data());
}

template <SoLib::IsNotPointer T, bool IsActive>
inline T *const CBuffer<T, IsActive>::operator->() const noexcept
{
	return reinterpret_cast<T *>(buffer_->data());
}

template <SoLib::IsNotPointer T, bool IsActive>
inline CBuffer<T, IsActive> &CBuffer<T, IsActive>::operator=(const T &other)
{
	buffer_->GetAccessor<T>()[0] = static_cast<T>(other);
	return *this;
}

template <SoLib::IsNotPointer T, bool IsActive>
inline CBuffer<T, IsActive>::CBuffer()
{
	CreateBuffer();
}

template <SoLib::IsNotPointer T, bool IsActive>
inline CBuffer<T, IsActive>::CBuffer(const CBuffer<T, IsActive> &other)
{
	CreateBuffer();

	// データのコピー
	buffer_->GetAccessor<T>()[0] = other.buffer_->GetAccessor<T>()[0];
}

template <SoLib::IsNotPointer T, bool IsActive>
inline CBuffer<T, IsActive>::CBuffer(CBuffer &&other)
{
	buffer_ = std::move(other.buffer_);
	other.buffer_ = {};
}

template <SoLib::IsNotPointer T, bool IsActive>
inline CBuffer<T, IsActive>::CBuffer(const T &r)
{
	CreateBuffer();

	// データのコピー
	buffer_->GetAccessor<T>()[0] = r;
}

template <SoLib::IsNotPointer T, bool IsActive>
inline void CBuffer<T, IsActive>::CreateBuffer()
{

	// メモリマネージャを取得
	auto *const bufferManager = SolEngine::DxResourceBufferPoolManager<>::GetInstance();

	// Tが格納できるサイズのバッファを作成して､そのハンドルを保存
	buffer_ = bufferManager->PushBack<T>();
}

template <SoLib::IsNotPointer T, bool IsActive>
inline CBuffer<T, IsActive>::~CBuffer()
{
	// resources_->Release();
}

template <SoLib::IsNotPointer T, bool IsActive>
inline CBuffer<T, IsActive> &CBuffer<T, IsActive>::operator=(CBuffer<T, IsActive> &&other)
{

	buffer_ = std::move(other);
	other.buffer_ = {};
	return *this;
}

#pragma endregion

#pragma region バッファを持たないクラス

/// @class CBuffer
/// @brief バッファ機能を持たないクラス
/// @details 実質的にポインタのラッパークラス
template <SoLib::IsNotPointer T>
class CBuffer<T, false>
{
public:
	CBuffer() = default;						   // デフォルトコンストラクタ
	CBuffer(const CBuffer &) = default;			   // コピーコンストラクタ
	CBuffer &operator=(const CBuffer &) = default; // コピー演算子

	~CBuffer() = default;

	inline operator bool() const noexcept; // 値が存在するか

	inline operator T &() noexcept;				// 参照
	inline operator const T &() const noexcept; // const参照

	inline T *const operator->() noexcept;		 // dataのメンバへのアクセス
	inline T *const operator->() const noexcept; // dataのメンバへのアクセス(const)

	inline CBuffer &operator=(const T &other); // コピー演算子

	inline void operator=(T *const ptr) { mapData_ = ptr; }

	/// @fn void SetMapAddress(T *const)
	/// @brief 書き込み先のアドレスを設定
	/// @param[in] ptr 書き込み先のアドレスを指定
	void SetMapAddress(T *const ptr) { mapData_ = ptr; }

	/// @fn T *const Get(void)
	/// @brief データの取得
	/// @return データのポインタ
	inline T *const Get() { return mapData_; }
	inline const T *const Get() const { return mapData_; }

private:
	T *mapData_;
};

template <SoLib::IsNotPointer T>
inline CBuffer<T, false>::operator bool() const noexcept
{
	return mapData_ != nullptr;
}

template <SoLib::IsNotPointer T>
inline CBuffer<T, false>::operator T &() noexcept
{
	return *mapData_;
}

template <SoLib::IsNotPointer T>
inline CBuffer<T, false>::operator const T &() const noexcept
{
	return *mapData_;
}

template <SoLib::IsNotPointer T>
inline T *const CBuffer<T, false>::operator->() noexcept
{
	return mapData_;
}

template <SoLib::IsNotPointer T>
inline T *const CBuffer<T, false>::operator->() const noexcept
{
	return mapData_;
}

template <SoLib::IsNotPointer T>
inline CBuffer<T, false> &CBuffer<T, false>::operator=(const T &other)
{
	if (mapData_) {
		*mapData_ = static_cast<const T &>(other);
	}
	return *this;
}

#pragma endregion

#pragma region mapアドレスを外部に持たせたクラス


/// @class ConstantContainer
/// @brief mapアドレスを外部に持たせたクラス
template <SoLib::IsNotPointer T>
class ConstantContainer 
{
public:
	// リソースとして保持する構造体
	using map_struct = typename T::map_struct;

private:
	// 静的警告
	static_assert(!std::is_pointer<T>::value, "CBufferに与えた型がポインタ型です");
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	// ComPtr<ID3D12Resource> resources_ = nullptr;

	CBuffer<map_struct> resource_;

	T data_;

public:
	inline ID3D12Resource *const GetResources() noexcept { return &resource_.GetResources(); }
	inline const ID3D12Resource *const GetResources() const noexcept { return &resource_.GetResources(); }

	inline const D3D12_CONSTANT_BUFFER_VIEW_DESC &GetView() const noexcept { return resource_.GetView(); }

	inline operator T &() noexcept { return data_; }			 // 参照
	inline operator const T &() const noexcept { return data_; } // const参照

	inline T *const operator->() noexcept { return &data_; }			 // dataのメンバへのアクセス
	inline const T *const operator->() const noexcept { return &data_; } // dataのメンバへのアクセス(const)

	inline T *const operator*() noexcept { return &data_; }				// dataのメンバへのアクセス
	inline const T *const operator*() const noexcept { return &data_; } // dataのメンバへのアクセス(const)

	inline T &operator=(const T &other)
	{ // コピー演算子
		data_ = static_cast<const T &>(other);
		if (data_.mapTarget_ && other.mapTarget_) {
			*data_.mapTarget_ = *other.mapTarget_;
		}
		return data_;
	}

public:
	inline D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const noexcept
	{
		return resource_.GetGPUVirtualAddress();
	}

public:
	ConstantContainer() { CreateBuffer(); };

	ConstantContainer &operator=(const ConstantContainer &other);
	~ConstantContainer() = default;

private:
	void CreateBuffer()
	{
		data_.mapTarget_ << resource_.get();
	}
};

/// @brief マッピングされるアドレスをコピーするためだけのクラス
/// @tparam T 保存する型
template <SoLib::IsNotPointer T>
class CMapTarget
{
public:
	CMapTarget() = default;
	CMapTarget(const CMapTarget &) {};
	CMapTarget &operator=(const CMapTarget &) { return *this; };
	~CMapTarget() = default;

	T *const operator=(T *const) = delete;
	T &operator=(const T &other)
	{
		if (target_) {
			*target_ = other;
		}
		return *target_;
	};
	T *const operator<<(T *const target) { return target_ = target; };

	inline operator T * const() { return target_; }
	inline operator const T * const() const { return target_; }

	inline T *const operator->() noexcept { return target_; }			  // dataのメンバへのアクセス
	inline const T *const operator->() const noexcept { return target_; } // dataのメンバへのアクセス(const)

	inline operator bool() const { return target_; }

private:
	T *target_ = nullptr;
};

#pragma endregion

template <SoLib::IsNotPointer T>
inline ConstantContainer<T> &ConstantContainer<T>::operator=(const ConstantContainer<T> &other)
{
	*this = other.data_;
	return *this;
}

template <SoLib::IsNotPointer T, bool IsActive>
template <typename U>
inline void CBuffer<T, IsActive>::SetName()
{

#ifdef USE_IMGUI

	/*if (resources_) {
		resources_->SetName(ConvertString(typeid(U).name()).c_str());
	}*/

#endif // USE_IMGUI
}
