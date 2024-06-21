#pragma once
#include "../Base/EngineObject.h"
#include <d3d12.h>
#include <algorithm>
#include <memory>

namespace SolEngine {
	class DxResource :protected EngineObject {
	public:

		DxResource() = default;
		DxResource(const DxResource &) = delete;
		DxResource &operator=(const DxResource &) = delete;

		DxResource(DxResource &&other) noexcept : resource_(std::move(other.resource_)), mapPtr_(other.mapPtr_) {}
		DxResource &operator=(DxResource &&other) noexcept { resource_ = std::move(other.resource_); return *this; }

		bool operator==(const DxResource &other) const = default;

	public:

		/// @brief データを構築する
		/// @param dataSize sizeof()で得られたクラスのサイズ
		/// @param dataCount データの数
		/// @return 生成されたデータ
		static DxResource Create(const uint32_t dataSize, const uint32_t dataCount = 1);


	private:

		ComPtr<ID3D12Resource> resource_;

		void *mapPtr_;

	};
}