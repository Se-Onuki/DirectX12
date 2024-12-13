/// @file MemoryUsageManager.h
/// @brief GPUメモリの管理を行う
/// @author ONUKI seiya
#pragma once
#include <stdint.h>
#include <vector>
#include <memory>

/// @class MemoryUsageManager
/// @brief コンテナの仕様領域の管理を行うクラス
class MemoryUsageManager {
public:
	MemoryUsageManager(size_t size = 0u) : dataBitset_(size), size_(size) {}

	/// @struct MemoryRange
	/// @brief メモリ範囲
	struct MemoryRange {
		MemoryRange &operator=(const MemoryRange &) = delete;
		~MemoryRange();
		uint32_t offset{};
		uint32_t length{};

		MemoryUsageManager *pManager_ = nullptr;

		/// @brief lengthが 1 以上なら有効
		inline operator bool() const { return length != 0u && pManager_ != nullptr; }
		/// @fn static MemoryRange Null(void)
		/// @brief 無効なMemoryRangeデータを生成する
		/// @return 無効なMemoryRange
		static MemoryRange Null() { return MemoryRange{ .offset = 0u,.length = 0u, .pManager_ = nullptr }; }
	};

	/// @fn std::unique_ptr<MemoryUsageManager::MemoryRange> RequestRange(uint32_t)
	/// @brief 指定された長さの連続した未使用領域の先頭インデックスを返す
	/// @param[in] length 使用する領域
	/// @return 先頭インデックス
	std::unique_ptr<MemoryUsageManager::MemoryRange> RequestRange(uint32_t length);

	/// @fn bool Free(const MemoryUsageManager::MemoryRange &)
	/// @brief メモリの解放
	/// @param[in] memory メモリ範囲
	/// @return	破棄が成功したか
	bool Free(const MemoryRange &memory);

	const size_t &size() const noexcept { return size_; }

private:
	const size_t size_;
	std::vector<bool> dataBitset_;
};
