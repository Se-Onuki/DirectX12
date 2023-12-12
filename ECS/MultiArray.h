#pragma once
#include "Archetype.h"
#include <vector>
#include <memory>
#include "../Utils/SoLib/SoLib_Traits.h"

namespace ECS {
	class MultiArray;

	template<SoLib::IsNotPointer T>
	class SubMultiArray;

	class MultiChunk {
		using memoryType = uint8_t;
	public:
		MultiChunk(MultiArray *const parent);

		template<SoLib::IsNotPointer T>
		T *const back() {
			return reinterpret_cast<T *>(componentAddress_[typeid(T)])[size_ - 1u];
		}

		/*void Delete(const int32_t index) {

		}*/

		template<SoLib::IsNotPointer T>
		SubMultiArray<T> GetSubArray();

		uint32_t size() const { return size_; }

		bool IsMax() const { return size_ >= archetype_->GetChunkCapacity(); }
		bool empty() const { return not size_; }

	private:
		uint32_t size_{};

		MultiArray *const parent_ = nullptr;
		const Archetype *const archetype_;
		std::unique_ptr<memoryType[]> componentArray_ = nullptr;

		std::unordered_map<std::type_index, void *> componentAddress_;
	};

	template<SoLib::IsNotPointer T>
	class SubMultiArray {
	public:
		SubMultiArray(T *begin, uint32_t size) : begin_(begin), size_(size) {}

		T &operator[](uint32_t index) { return begin_[index]; }
		const T &operator[](uint32_t index) const { return begin_[index]; }


		T *begin() { return begin_; }
		const T *begin() const { return begin_; }
		const T *cbegin() const { return begin_; }

		T *end() { return begin_ + size_; }
		const T *end() const { return begin_ + size_; }
		const T *cend() const { return begin_ + size_; }
	private:
		T *begin_ = nullptr;
		uint32_t size_;

	};

	class MultiArray {
	public:
		MultiArray(const Archetype &archetype) : archetype_(archetype) { AddChunk(); }
		~MultiArray() = default;

		const Archetype &GetArchetype() const { return archetype_; }

		void AddChunk();

		template<SoLib::IsNotPointer T>
		T &GetItem(uint32_t totalIndex);


	private:
		Archetype archetype_;
		std::vector<std::unique_ptr<MultiChunk>> multiChunk_;
	};


#pragma region Func

	template<SoLib::IsNotPointer T>
	inline SubMultiArray<T> MultiChunk::GetSubArray() {
		return SubMultiArray{ reinterpret_cast<T *>(componentAddress_.at(typeid(T))), size_ };
	}


	template<SoLib::IsNotPointer T>
	T &MultiArray::GetItem(uint32_t totalIndex) {
		const auto capacity = archetype_.GetChunkCapacity();

		return multiChunk_[totalIndex / capacity]->GetSubArray<T>()[totalIndex % capacity];

	}


#pragma endregion

}