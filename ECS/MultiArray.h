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
		public:
			iterator() = default;
			iterator(ComponetArray *const, uint32_t);

			iterator &operator =(const iterator &) = default;
			iterator &operator =(iterator &&) = default;

			std::tuple<Ts *const...> operator *();

			iterator &operator++() {

				this->index_++;

				return *this;
			}

			iterator operator+(uint32_t index) {
				iterator result = *this;
				result.index_ + index;

				return result;
			}

			iterator &operator+=(uint32_t index) {
				this->index_ += index;

				return *this;
			}

			bool operator==(const iterator &other) const {
				return this->compArray_ == other.compArray_ && this->index_ == other.index_;
			}

			bool operator!=(const iterator &other) const {
				return this->compArray_ != other.compArray_ || this->index_ != other.index_;
			}

		private:
			ComponetArray *compArray_;
			uint32_t index_{};

		};



		bool operator!=(const ComponetArray<Ts...> &other) const {
			return this->componentAddress_ != other.componentAddress_ || this->size_ != other.size_;
		}

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
		using MultiChunkClass = std::vector<std::unique_ptr<MultiChunk>>;

		template<typename... Ts>
		class MultiCompArray {
		public:

			MultiCompArray(MultiChunkClass *const multiChunk) :pMultiChunk_(multiChunk) {}
			~MultiCompArray() = default;

			class iterator {
			public:
				iterator() = default;
				iterator(MultiChunkClass *const multiChunk, MultiChunkClass::iterator chunkItr, uint32_t index) :pMultiChunk_(multiChunk), chunkItr_(chunkItr) {
					if (chunkItr_ != pMultiChunk_->end()) {
						compArray_ = (*chunkItr_)->get<Ts...>();
						compArrayItr_ = compArray_.begin();
						compArrayItr_ += index;
					}
				}
				~iterator() = default;

				iterator &operator++();

				std::tuple<Ts *const...> operator *() {
					return *(this->compArrayItr_);
				}

				bool operator==(const iterator &other) const {
					return this->pMultiChunk_ == other.pMultiChunk_ &&
						this->chunkItr_ == other.chunkItr_ &&
						this->compArray_ == other.compArray_ &&
						this->compArrayItr_ == other.compArrayItr_;
				}

				bool operator!=(const iterator &other) const {
					return this->pMultiChunk_ != other.pMultiChunk_ ||
						this->chunkItr_ != other.chunkItr_ ||
						this->compArray_ != other.compArray_ ||
						this->compArrayItr_ != other.compArrayItr_;
				}


			private:
				MultiChunkClass *pMultiChunk_;
				MultiChunkClass::iterator chunkItr_;
				ComponetArray<Ts...> compArray_;
				ComponetArray<Ts...>::iterator compArrayItr_;
			};

			iterator begin();
			iterator end();

		private:
			MultiChunkClass *const pMultiChunk_;
		};


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

		template<typename... Ts>
		MultiCompArray<Ts...> get();

	private:
		Archetype archetype_;
		MultiChunkClass multiChunk_;
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
	inline  std::tuple<Ts *const...> ComponetArray<Ts...>::iterator::operator*() {
		return std::make_tuple(&(static_cast<Ts *const>(compArray_->componentAddress_.at(typeid(Ts)))[index_])...);
	}

	template<typename ...Ts>
	inline MultiArray::MultiCompArray<Ts...>::iterator MultiArray::MultiCompArray<Ts...>::begin() {
		return iterator(this->pMultiChunk_, this->pMultiChunk_->begin(), 0u);
	}

	template<typename ...Ts>
	inline MultiArray::MultiCompArray<Ts...>::iterator MultiArray::MultiCompArray<Ts...>::end() {

		return iterator(this->pMultiChunk_, this->pMultiChunk_->end(), 0u);
	}
	template<typename ...Ts>
	inline MultiArray::MultiCompArray<Ts...>::iterator &MultiArray::MultiCompArray<Ts...>::iterator::operator++() {

		// 内部イテレータを加算
		++compArrayItr_;
		// もし、末尾に到達していたらチャンクを加算
		if (compArrayItr_ == compArray_.end()) {
			++this->chunkItr_;

			// チャンクが終端以外は内部データを更新
			if (chunkItr_ != pMultiChunk_->end()) {
				compArray_ = (*chunkItr_)->get<Ts...>();
				compArrayItr_ = compArray_.begin();
			}

		}


		return *this;
	}



	template<typename ...T>
	inline MultiArray::MultiCompArray<T...> MultiArray::get() {
		MultiArray::MultiCompArray<T...> result{ &this->multiChunk_ };

		return result;
	}

}