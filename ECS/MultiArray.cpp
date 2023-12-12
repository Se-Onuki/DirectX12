#include "MultiArray.h"

ECS::MultiChunk::MultiChunk(MultiArray *const parent) : parent_(parent), archetype_(&parent_->GetArchetype()) {
	// メモリの確保
	componentArray_ = std::make_unique<memoryType[]>(Archetype::OneChunkCapacity / sizeof(memoryType));

	size_t capacity = archetype_->GetChunkCapacity();

	memoryType *address = componentArray_.get() + sizeof(ECS::Entity) * capacity / sizeof(memoryType);
	for (const auto &classData : archetype_->data_) {
		componentAddress_[classData.typeInfo_] = address;
		address += (classData.size_ * capacity / sizeof(memoryType));
	}

}

void ECS::MultiArray::AddChunk() {
	multiChunk_.push_back(std::make_unique<ECS::MultiChunk>(this));
}
