#include "MemoryUsageManager.h"

std::shared_ptr<MemoryUsageManager::MemoryRange> MemoryUsageManager::RequestRange(uint32_t length) {
	for (uint32_t i = 0; i < size_ - length + 1; ++i) {
		if (dataBitset_[i] == false) {
			bool isFree = true;
			for (uint32_t j = i; j < i + length; ++j) {
				if (dataBitset_[j]) {
					isFree = false;
					break;
				}
			}
			if (isFree) {
				// 領域が見つかった場合、該当領域をマークしてインデックスを返す
				for (uint32_t j = i; j < i + length; ++j) {
					dataBitset_[j] = true;
				}
				return std::make_shared<MemoryRange>(i, length, this);
			}
		}
	}
	// 連続した未使用領域が見つからない場合、無効なデータを返す。
	return std::make_shared<MemoryRange>(MemoryRange::Null());
}

bool MemoryUsageManager::Free(const MemoryRange &memory) {
	// マネージャが一致していない場合は無効
	if (memory.pManager_ != this) { return false; }
	//	領域が無効なデータであった場合、無効
	if (not memory) { return false; }

	// 指定された範囲を無効に
	for (uint32_t i = 0u; i < memory.length; ++i) {
		dataBitset_[memory.offset + i] = false;
	}
	return true;
}

MemoryUsageManager::MemoryRange::~MemoryRange() {
	if (not *this) { return; }
	this->pManager_->Free(*this);
}
