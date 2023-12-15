#pragma once
#include "Archetype.h"
#include <vector>
#include <memory>
#include <cstdint>
#include "../Utils/SoLib/SoLib_Traits.h"

namespace ECS {
	class MultiArray;

	template<SoLib::IsNotPointer T>
	class SubMultiArray;

	class MultiChunk;


	struct ChunkDeleter {
		void operator()(void *p) const {
			// メモリを解放
			operator delete(p);
		}
	};


	template<typename... Ts>
	struct ComponetArray {

		struct iterator {

			iterator(ComponetArray *const, uint32_t);
			//operator std::tuple<Ts*...>();
			const std::tuple<Ts *const...> operator *();

			iterator &operator++() {

				this->index_++;

				return *this;
			}

			bool operator==(const iterator &other) {
				return this->compArray_ == other.compArray_ && this->index_ == other.index_;
			}

			bool operator!=(const iterator &other) {
				return this->compArray_ != other.compArray_ || this->index_ != other.index_;
			}

		private:
			ComponetArray *const compArray_;
			uint32_t index_{};

		};


		iterator begin() { return iterator{ this, 0u }; }
		iterator end() { return iterator{ this, size_ }; }

		std::unordered_map<std::type_index, void *> componentAddress_;
		uint32_t size_;
	};

	class MultiChunk {
		using memoryType = uint8_t;
	public:
		MultiChunk(MultiArray *const parent);

		template<SoLib::IsNotPointer T>
		T *const back() {
			return reinterpret_cast<T *>(componentAddress_[typeid(T)])[size_ - 1u];
		}

		/// @brief データの置き換えによるデータの破棄
		/// @param index 破棄するデータの内部index
		void erese(uint32_t index);

		void swap(const uint32_t indexF, const uint32_t indexS);

		/// @brief 末尾へのデータの追加
		/// @return 追加されたデータの内部index
		uint32_t push_back();

		template<SoLib::IsNotPointer T>
		SubMultiArray<T> GetSubArray();

		uint32_t size() const { return size_; }

		void Normalize();

		bool IsMax() const { return size_ >= archetype_->GetChunkCapacity(); }
		bool empty() const { return not size_; }

		void *GetItemPtr(const ClassData &classData, const uint32_t index);

		template<typename... Ts>
		ComponetArray<Ts...> get();

	private:
		uint32_t size_{};

		MultiArray *const parent_ = nullptr;
		const Archetype *const archetype_;

		std::unique_ptr<void, ChunkDeleter> memoryPtr_;

		std::unordered_map<ClassData, void *> componentAddress_;
	};

	template<SoLib::IsNotPointer T>
	class SubMultiArray {
	public:
		SubMultiArray(T *begin, uint32_t size) : begin_(begin), size_(size) {}

		T &operator[](uint32_t index) {
			assert(size_ > index);
			return begin_[index];
		}
		const T &operator[](uint32_t index) const {
			assert(size_ > index);
			return begin_[index];
		}


		T *begin() { return begin_; }
		const T *begin() const { return begin_; }
		const T *cbegin() const { return begin_; }

		T *end() { return begin_ + size_; }
		const T *end() const { return begin_ + size_; }
		const T *cend() const { return begin_ + size_; }

		uint32_t size() const { return size_; }
	private:
		T *begin_ = nullptr;
		uint32_t size_;

	};

	class MultiArray {
	public:
		MultiArray(const Archetype &archetype) : archetype_(archetype) { }
		~MultiArray() = default;

		const Archetype &GetArchetype() const { return archetype_; }

		std::unique_ptr<MultiChunk> &AddChunk();

		auto &GetChunk() { return multiChunk_; }

		/// @brief 末尾に要素を追加
		/// @return 追加された要素のindex
		size_t push_back();

		template<SoLib::IsNotPointer T>
		T &GetItem(size_t totalIndex);

		/// @brief 穴抜けの部分を埋める
		void Normalize();

		/// @brief データの破棄
		/// @param totalIndex 破棄するindex
		void erese(const uint32_t totalIndex);

		void swap(const uint32_t totalIndexF, const uint32_t totalIndexS);

	private:
		Archetype archetype_;
		std::vector<std::unique_ptr<MultiChunk>> multiChunk_;
	};


#pragma region Func

	template<SoLib::IsNotPointer T>
	inline SubMultiArray<T> MultiChunk::GetSubArray() {
		return SubMultiArray{ reinterpret_cast<T *>(componentAddress_.at(ClassData{typeid(T),sizeof(T)})), size_ };
	}

	template<typename ...T>
	inline ComponetArray<T...> MultiChunk::get() {
		ComponetArray<T...> result;
		for (auto &[comp, ptr] : this->componentAddress_) {
			result.componentAddress_.insert(std::make_pair(comp.typeInfo_, ptr));
		}
		result.size_ = size_;

		return result;
	}


	template<SoLib::IsNotPointer T>
	T &MultiArray::GetItem(size_t totalIndex) {
		const auto capacity = archetype_.GetChunkCapacity();

		return multiChunk_[totalIndex / capacity]->GetSubArray<T>()[static_cast<uint32_t>(totalIndex % capacity)];

	}


#pragma endregion

	/*template<typename ...Ts>
	inline ComponetArray<Ts...>::iterator::operator std::tuple<Ts*...>() {
		std::tuple<Ts*...> result = std::make_tuple(&(static_cast<Ts *>(compArray_->componentAddress_.at(typeid(Ts)))[index_])...);

		return result;
	}*/



	template<typename ...Ts>
	inline ComponetArray<Ts...>::iterator::iterator(ComponetArray *const compArray, uint32_t index) :compArray_(compArray) {
		index_ = index;
	}

	template<typename ...Ts>
	inline const std::tuple<Ts *const...> ComponetArray<Ts...>::iterator::operator*() {
		return std::make_tuple(&(static_cast<Ts *const>(compArray_->componentAddress_.at(typeid(Ts)))[index_])...);
	}

}