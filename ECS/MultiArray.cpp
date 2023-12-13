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

uint32_t ECS::MultiChunk::push_back() {

	for (const auto &classData : archetype_->data_) {
		auto ptr = reinterpret_cast<memoryType *>(componentAddress_[classData]) + classData.size_ / sizeof(memoryType) * size_;
		classData.constructor_(ptr);
	}

	return size_++;
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
