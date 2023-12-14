#include "MultiArray.h"

ECS::MultiChunk::MultiChunk(MultiArray *const parent) : parent_(parent), archetype_(&parent_->GetArchetype()) {
	// メモリの確保
	memoryPtr_.reset(operator new(Archetype::OneChunkCapacity));

	size_t capacity = archetype_->GetChunkCapacity();

	memoryType *address = reinterpret_cast<memoryType *>(memoryPtr_.get()) + sizeof(ECS::Entity) * capacity / sizeof(memoryType);
	for (const auto &classData : archetype_->data_) {
		componentAddress_[classData] = address;
		address += (classData.size_ * capacity / sizeof(memoryType));
	}

}

void ECS::MultiChunk::erese(uint32_t index) {
	for (const auto &classData : archetype_->data_) {
		// 破棄するデータのアドレスを取得
		auto ptr = GetItemPtr(classData, index);
		// 末尾のデータのアドレスをを取得
		auto backPtr = GetItemPtr(classData, size_ - 1u);
		// 破棄するデータを末尾のデータで上書き
		std::memcpy(ptr, backPtr, classData.size_);

	}
	// 配列のサイズを短縮
	size_--;
}

uint32_t ECS::MultiChunk::push_back() {

	for (const auto &classData : archetype_->data_) {
		auto ptr = GetItemPtr(classData, size_);
		classData.constructor_(ptr);
	}

	return size_++;
}

void ECS::MultiChunk::Normalize() {

}

void *ECS::MultiChunk::GetItemPtr(const ClassData &classData, const uint32_t index) {
	return reinterpret_cast<memoryType *>(componentAddress_[classData]) + classData.size_ / sizeof(memoryType) * index;
}

std::unique_ptr<ECS::MultiChunk> &ECS::MultiArray::AddChunk() {
	multiChunk_.push_back(std::make_unique<ECS::MultiChunk>(this));
	return multiChunk_.back();
}

size_t ECS::MultiArray::push_back() {

	auto backChunk = multiChunk_.rbegin();

	size_t index{};

	// 最後尾が最大であるか
	if (multiChunk_.empty() || (*backChunk)->IsMax()) {
		// 最大ならチャンクを追加して、エンティティを追加
		index = this->AddChunk()->push_back();
	}
	else {
		// 余裕があるなら、そのままエンティティを追加。
		index = (*backChunk)->push_back();
	}

	return index + (multiChunk_.size() - 1u) * archetype_.GetChunkCapacity();
}

void ECS::MultiArray::Normalize() {

}

void ECS::MultiArray::erese(const uint32_t totalIndex) {
	const auto capacity = archetype_.GetChunkCapacity();

	multiChunk_[totalIndex / capacity]->erese(static_cast<uint32_t>(totalIndex % capacity));
}
