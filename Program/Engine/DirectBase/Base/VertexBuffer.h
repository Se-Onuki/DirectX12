#pragma once

#include "StructBuffer.h"
#include "../../../Utils/SoLib/SoLib_Traits.h"

#pragma region 頂点バッファ

/// @brief 頂点バッファ
/// @tparam T 頂点データの型 Index 添え字が有効か
template <SoLib::IsRealType T, bool Index = true>
class VertexBuffer final {

	ArrayBuffer<T> vertexData_;
	D3D12_VERTEX_BUFFER_VIEW vbView_;

	ArrayBuffer<uint32_t> indexData_;
	D3D12_INDEX_BUFFER_VIEW ibView_;

public:
	VertexBuffer() = default;
	VertexBuffer(const VertexBuffer &) = default;
	~VertexBuffer() = default;

	auto &GetVertexData() noexcept { return vertexData_; }
	const auto &GetVertexData() const noexcept { return vertexData_; }
	const auto &GetVBView() const noexcept { return vbView_; };

	auto &GetIndexData() noexcept { return indexData_; }
	const auto &GetIndexData() const noexcept { return indexData_; }
	const auto &GetIBView() const noexcept { return ibView_; };

	template <SoLib::IsContainsType<T> U>
	void SetVertexData(const U &source);
	template <SoLib::IsContainsType<uint32_t> U>
	void SetIndexData(const U &source);
};

template <SoLib::IsRealType T, bool Index>
template <SoLib::IsContainsType<T> U>
void VertexBuffer<T, Index>::SetVertexData(const U &source) {
	vertexData_ = source;

	// 頂点バッファビューを作成する
	// リソースの先頭のアドレスから使う
	vbView_.BufferLocation = vertexData_.GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点3つ分のサイズ
	vbView_.SizeInBytes = static_cast<UINT>(sizeof(T) * vertexData_.size());
	// 1頂点あたりのサイズ
	vbView_.StrideInBytes = sizeof(T);

}

template <SoLib::IsRealType T, bool Index>
template <SoLib::IsContainsType<uint32_t> U>
void VertexBuffer<T, Index>::SetIndexData(const U &source) {

	indexData_ = source;

	// インデックスview
	ibView_.BufferLocation = indexData_.GetGPUVirtualAddress();
	ibView_.SizeInBytes = static_cast<UINT>(sizeof(uint32_t) * indexData_.size());
	ibView_.Format = DXGI_FORMAT_R32_UINT;
}

#pragma endregion
