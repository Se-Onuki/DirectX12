#include "MultiArray.h"
#include <cassert>
#include <algorithm>

ECS::MultiChunk::MultiChunk(MultiArray *const parent) : parent_(parent), archetype_(&parent_->GetArchetype()) {
	// メモリの確保
	memoryPtr_.reset(operator new(Archetype::OneChunkCapacity));

	//size_t capacity = archetype_->GetChunkCapacity();
	//size_t offset = capacity / sizeof(memoryType);
	size_t offset = 1u;
	entitySize_ = static_cast<uint32_t>(archetype_->GetTotalSize());

	memoryType *address = reinterpret_cast<memoryType *>(memoryPtr_.get()) + sizeof(ECS::Entity) * offset;
	for (const auto &classData : archetype_->data_) {
		componentAddress_[classData.first] = std::make_pair(address, classData.second);
		address += (classData.second.size_ * offset);
	}

}

void ECS::MultiChunk::erase(uint32_t index) {
	if (size_ > index) {
		//size_t totalSize = archetype_->GetTotalSize();
		//for (const auto &classData : archetype_->data_) {
			// 破棄するデータのアドレスを取得
			//auto ptr = GetItemPtr(classData.typeInfo_, index);
		auto ptr = GetEntityPtr(index);
		// 末尾のデータのアドレスをを取得
		//auto backPtr = GetItemPtr(classData.typeInfo_, size_ - 1u);
		auto backPtr = GetEntityPtr(size_ - 1u);
		// 破棄するデータを末尾のデータで上書き
		std::memcpy(ptr, backPtr, entitySize_);

		//}
		// 配列のサイズを短縮
		size_--;
	}
}

void ECS::MultiChunk::swap(const uint32_t indexF, const uint32_t indexS) {
#ifdef _DEBUG

	assert(size_ > indexF && "添え字がアクセス範囲を超えています");
	assert(size_ > indexS && "添え字がアクセス範囲を超えています");

#endif // _DEBUG

	size_t maxMemSize{};
	for (const auto &classData : archetype_->data_) {
		maxMemSize = (std::max)(maxMemSize, classData.second.size_);

	}
	// 一時的なメモリを最大値で確保
	std::unique_ptr<char[]> temp(std::make_unique<char[]>(maxMemSize));

	for (const auto &classData : archetype_->data_) {

		// 破棄するデータのアドレスを取得
		auto fPtr = GetItemPtr(classData.first, indexF);
		// 末尾のデータのアドレスをを取得
		auto sPtr = GetItemPtr(classData.first, indexS);

		std::memcpy(temp.get(), fPtr, classData.second.size_);
		std::memcpy(fPtr, sPtr, classData.second.size_);
		std::memcpy(sPtr, temp.get(), classData.second.size_);

	}

}

uint32_t ECS::MultiChunk::push_back() {

	for (const auto &classData : archetype_->data_) {
		auto ptr = GetItemPtr(classData.first, size_);
		classData.second.constructor_(ptr);
	}

	return size_++;
}

uint32_t ECS::MultiChunk::push_back(const ECS::Prefab &prefab) {

	auto compItr = prefab.GetComponentMap().begin();

	for (const auto &[typeindex, classData] : archetype_->data_) {
		auto ptr = GetItemPtr(typeindex, size_);
		std::memcpy(ptr, compItr->second.get(), classData.size_);
		++compItr;
	}

	return size_++;
}

uint32_t ECS::MultiChunk::pop_back() {
	if (size_ > 0u) {
		return --size_;
	}
	return 0u;
}

void ECS::MultiChunk::Normalize() {

}

void *ECS::MultiChunk::GetItemPtr(const std::type_index type, const uint32_t index) {
	const auto &[itemPtr, classData] = componentAddress_.at(type);

	return reinterpret_cast<memoryType *>(itemPtr) + entitySize_ * index;
}

void *ECS::MultiChunk::GetEntityPtr(const uint32_t index) {
	return reinterpret_cast<memoryType *>(this->memoryPtr_.get()) + entitySize_ * index;
}

ECS::MultiChunk *ECS::MultiArray::AddChunk() {
	multiChunk_.push_back(std::make_unique<ECS::MultiChunk>(this));
	return multiChunk_.back().get();
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

size_t ECS::MultiArray::push_back(const ECS::Prefab &prefab) {

	auto backChunk = multiChunk_.rbegin();

	size_t index{};

	// 最後尾が最大であるか
	if (multiChunk_.empty() || (*backChunk)->IsMax()) {
		// 最大ならチャンクを追加して、エンティティを追加
		index = this->AddChunk()->push_back(prefab);
	}
	else {
		// 余裕があるなら、そのままエンティティを追加。
		index = (*backChunk)->push_back(prefab);
	}

	return index + (multiChunk_.size() - 1u) * archetype_.GetChunkCapacity();
}

void ECS::MultiArray::Normalize() {

	// チャンクが1つ以上なら実行
	if (multiChunk_.size()) {
		// チャンクデータが空なら破棄
		while (multiChunk_.size() && multiChunk_.back()->empty()) {
			multiChunk_.pop_back();
		}
		// チャンクの最大値
		const size_t capacity = archetype_.GetChunkCapacity();
		// 現在のチャンクID
		uint32_t chunkID = 0u;
		// 各チャンクを回す

		auto chunkItr = multiChunk_.begin();
		while (chunkItr != multiChunk_.end()) {

			if (not (*chunkItr)->IsMax()) {

				for (uint32_t i = (*chunkItr)->size(); i < capacity; i++) {
					auto &back = multiChunk_.back();
					// もし末尾と現在のチャンクが一致したら終了
					if (back == (*chunkItr)) { break; }

					this->swap(chunkID * capacity + (*chunkItr)->push_back(), (multiChunk_.size() - 1u) * capacity + back->pop_back());
					while (multiChunk_.back()->empty()) {
						multiChunk_.erase(--multiChunk_.end());
					}
				}
			}

			++chunkID;
			++chunkItr;
		}

		//for (auto &chunk : multiChunk_) {
		//	// チャンクが最大値じゃないならば
		//	if (not chunk->IsMax()) {
		//
		//		for (uint32_t i = chunk->size(); i < capacity; i++) {
		//			auto &back = multiChunk_.back();
		//			// もし末尾と現在のチャンクが一致したら終了
		//			if (back == chunk) { break; }
		//
		//			this->swap(chunkID * capacity + chunk->push_back(), multiChunk_.size() * capacity + back->pop_back());
		//			while (multiChunk_.back()->empty()) {
		//				multiChunk_.erase(--multiChunk_.end());
		//			}
		//		}
		//	}
		//	chunkID++;
		//}
	}
}

void ECS::MultiArray::erase(const size_t totalIndex) {
	const auto capacity = archetype_.GetChunkCapacity();

	multiChunk_[totalIndex / capacity]->erase(static_cast<uint32_t>(totalIndex % capacity));
}

size_t ECS::MultiArray::size() const {
	// チャンクの数
	size_t chunkCount = this->multiChunk_.size();
	// もしチャンクが空なら
	if (not chunkCount) {
		return 0u;
	}
	else {
		size_t chunkCapacity = archetype_.GetChunkCapacity();
		return (chunkCount - 1u) * chunkCapacity + multiChunk_.back()->size();
	}
}

void ECS::MultiArray::swap(const size_t totalIndexF, const size_t totalIndexS) {
	const auto capacity = archetype_.GetChunkCapacity();
	const auto entitySize = archetype_.GetTotalSize();

	auto &firstItem = multiChunk_[totalIndexF / capacity];
	auto &secondItem = multiChunk_[totalIndexS / capacity];

	// 一時的なメモリを最大値で確保
	std::unique_ptr<char[]> temp(std::make_unique<char[]>(entitySize));

	//for (const auto &classData : archetype_.data_) {

		// 破棄するデータのアドレスを取得
	auto fPtr = firstItem->GetEntityPtr(static_cast<uint32_t>(totalIndexF % capacity));
	// 末尾のデータのアドレスをを取得
	auto sPtr = secondItem->GetEntityPtr(static_cast<uint32_t>(totalIndexS % capacity));

	std::memcpy(temp.get(), fPtr, entitySize);
	std::memcpy(fPtr, sPtr, entitySize);
	std::memcpy(sPtr, temp.get(), entitySize);

	//}

}