/// @file VertexBuffer.h
/// @brief 頂点バッファ
/// @author ONUKI seiya
#pragma once

#include "StructBuffer.h"
#include "../../Engine/Utils/SoLib/SoLib_Traits.h"

#pragma region 頂点バッファ

/// @brief 頂点バッファ
/// @tparam T 頂点データの型
template <SoLib::IsRealType T, D3D12_HEAP_TYPE HeapType = D3D12_HEAP_TYPE_UPLOAD>
class VertexBuffer final {

	ArrayBuffer<T, HeapType> vertexData_;
	D3D12_VERTEX_BUFFER_VIEW vbView_;

public:
	VertexBuffer() = default;
	VertexBuffer(const VertexBuffer &) = default;
	~VertexBuffer() = default;

	/// @brief 頂点バッファをリサイズする
	/// @param size 頂点数
	void Resize(uint32_t size) {
		vertexData_.CreateBuffer(size);

		// 頂点バッファビューを作成する
		// リソースの先頭のアドレスから使う
		vbView_.BufferLocation = vertexData_.GetGPUVirtualAddress();
		// 使用するリソースのサイズは頂点3つ分のサイズ
		vbView_.SizeInBytes = static_cast<UINT>(sizeof(T) * vertexData_.size());
		// 1頂点あたりのサイズ
		vbView_.StrideInBytes = sizeof(T);
	}

	T &operator[](const uint32_t i) { return vertexData_.data()[i]; }
	const T &operator[](const uint32_t i) const { return vertexData_.data()[i]; }

	auto &GetVertexData() noexcept { return vertexData_; }
	const auto &GetVertexData() const noexcept { return vertexData_; }
	const auto &GetVBView() const noexcept { return vbView_; };

	template <SoLib::IsContainsType<T> U>
	void SetVertexData(const U &source);
};

template <SoLib::IsRealType T, D3D12_HEAP_TYPE HeapType>
template <SoLib::IsContainsType<T> U>
void VertexBuffer<T, HeapType>::SetVertexData(const U &source) {
	vertexData_ = source;

	// 頂点バッファビューを作成する
	// リソースの先頭のアドレスから使う
	vbView_.BufferLocation = vertexData_.GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点3つ分のサイズ
	vbView_.SizeInBytes = static_cast<UINT>(sizeof(T) * vertexData_.size());
	// 1頂点あたりのサイズ
	vbView_.StrideInBytes = sizeof(T);

}

/// @brief Indexバッファ
/// @tparam T Indexデータの型
template <SoLib::IsIntegral T, D3D12_HEAP_TYPE HeapType = D3D12_HEAP_TYPE_UPLOAD>
class IndexBuffer final {

	ArrayBuffer<T, HeapType> indexData_;
	D3D12_INDEX_BUFFER_VIEW ibView_;

public:
	IndexBuffer() = default;
	IndexBuffer(const IndexBuffer &) = default;
	~IndexBuffer() = default;

	/// @brief Indexバッファをリサイズする
	/// @param size 頂点数
	void Resize(uint32_t size) {
		indexData_.CreateBuffer(size);
		// インデックスview
		ibView_.BufferLocation = indexData_.GetGPUVirtualAddress();
		ibView_.SizeInBytes = static_cast<UINT>(sizeof(T) * indexData_.size());
		ibView_.Format = DXGI_FORMAT_R32_UINT;
	}

	auto &GetIndexData() noexcept { return indexData_; }
	const auto &GetIndexData() const noexcept { return indexData_; }
	const auto &GetIBView() const noexcept { return ibView_; };

	T &operator[](const uint32_t i) { return indexData_.data()[i]; }
	const T &operator[](const uint32_t i) const { return indexData_.data()[i]; }

	template <SoLib::IsContainsType<T> U>
	void SetIndexData(const U &source);
};

template <SoLib::IsIntegral T, D3D12_HEAP_TYPE HeapType>
template <SoLib::IsContainsType<T> U>
void IndexBuffer<T, HeapType>::SetIndexData(const U &source) {

	indexData_ = source;

	// インデックスview
	ibView_.BufferLocation = indexData_.GetGPUVirtualAddress();
	ibView_.SizeInBytes = static_cast<UINT>(sizeof(T) * indexData_.size());
	ibView_.Format = DXGI_FORMAT_R32_UINT;
}

#pragma endregion
