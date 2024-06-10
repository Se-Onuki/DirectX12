#pragma once
#include "Archetype.h"
#include <vector>
#include <memory>
#include <cstdint>
#include "Entity/Prefab.h"
#include "../Utils/SoLib/SoLib_Traits.h"

namespace ECS {
	class MultiArray;

	class MultiChunk;


	struct ChunkDeleter {
		void operator()(void *p) const {
			// メモリを解放
			operator delete(p);
		}
	};


	//template<typename... Ts>
	//struct ComponentMiniArray {

	//	struct iterator {
	//		iterator() = default;
	//		iterator(ComponentMiniArray *const parent, uint32_t index) {
	//			itemPtr_ = parent->itemPtr_;
	//			itemCount_ = parent->itemCount_;
	//			index_ = index;
	//		}

	//		std::tuple<Ts *...> operator*() {
	//			std::tuple<Ts*...> result = itemPtr_;

	//			//std::apply([index_](auto&... args)
	//			//	{
	//			//		(args += index_, ...);
	//			//	}, result);

	//			return result;
	//		}

	//		iterator &operator++() {
	//			++index_;
	//			return *this;
	//		}

	//		bool operator !=(const iterator &other) {
	//			return this->itemPtr_ != other.itemPtr_ ||
	//				this->itemCount_ != other.itemCount_ ||
	//				this->index_ != other.index_;
	//		}


	//		std::shared_ptr<std::tuple<Ts *...>> itemPtr_;
	//		uint32_t itemCount_;
	//		uint32_t index_;
	//	};

	//	iterator begin() { return iterator{ this,0u }; }
	//	iterator end() { return iterator{ this,itemCount_ }; }
	//	iterator operator[](const uint32_t index) { return iterator{ this,index }; }

	//	std::shared_ptr<std::tuple<Ts *...>> itemPtr_;
	//	uint32_t itemCount_;

	//};


	template<typename... TComps>
	struct ComponetArray {

		struct iterator {
		public:
			iterator() = default;
			iterator(const iterator &) = default;
			iterator(ComponetArray *const, uint32_t);

			iterator &operator =(const iterator &) = default;
			iterator &operator =(iterator &&) = default;

			std::tuple<ECS::Entity *, TComps *...> operator *();

			iterator &operator++() {

				this->index_++;

				return *this;
			}

			iterator &operator--() {

				this->index_--;

				return *this;
			}

			iterator operator[](uint32_t index) {
				iterator result = *this;
				result.index_ += index;

				return result;
			}

			iterator operator+(uint32_t index) {
				iterator result = *this;
				result.index_ += index;

				return result;
			}

			iterator &operator+=(uint32_t index) {
				this->index_ += index;

				return *this;
			}

			bool operator==(const iterator &other) const {
				return this->componentAddress_ == other.componentAddress_ && this->index_ == other.index_;
			}

			bool operator!=(const iterator &other) const {
				return  this->componentAddress_ != other.componentAddress_ || this->index_ != other.index_;
			}

			uint32_t GetIndex() const { return index_; }

		private:

		private:
			std::shared_ptr<std::tuple<TComps *...>> componentAddress_;
			uint32_t index_{};
			size_t entitySize_{};
			MultiChunk *multiChunk_ = nullptr;

		};



		bool operator!=(const ComponetArray<TComps...> &other) const {
			if (not this->componentAddress_ && not other.componentAddress_) { return false; }
			return this->componentAddress_ != other.componentAddress_ || this->size_ != other.size_;
		}

		iterator begin() { return iterator{ this, 0u }; }
		iterator end() { return iterator{ this, size_ }; }

		iterator operator[](const uint32_t index) { return iterator{ this,index }; }

		std::shared_ptr<std::tuple<TComps *...>> componentAddress_;
		uint32_t size_;
		size_t entitySize_{};
		MultiChunk *multiChunk_ = nullptr;
	};

	class MultiChunk {
	public:
		using memoryType = uint8_t;
		MultiChunk(MultiArray *const parent);

		/// @brief データの置き換えによるデータの破棄
		/// @param index 破棄するデータの内部index
		void erase(uint32_t index);

		void swap(const uint32_t indexF, const uint32_t indexS);

		/// @brief 末尾へのデータの追加
		/// @return 追加されたデータの内部index
		uint32_t push_back();

		/// @brief 末尾へのデータの追加
		/// @return 追加されたデータの内部index
		uint32_t push_back(const ECS::Prefab &prefab);

		uint32_t pop_back();

		uint32_t size() const { return size_; }

		void Normalize();

		bool IsMax() const { return size_ >= archetype_->GetChunkCapacity(); }
		bool empty() const { return not size_; }

		void *GetItemPtr(const std::type_index type, const uint32_t index);

		void *GetEntityPtr(const uint32_t index);

		template<typename... TComps>
		ComponetArray<TComps...> get();

		template<typename... T>
		std::tuple<T *...> GetItem(const uint32_t index);

		template<typename T>
		T &GetOneItem(const uint32_t index);

		template<typename T, typename...TComps>
		void erase_if(const std::function <bool(ECS::Entity *, T *, TComps *...)> &func) {
			auto getItem = this->get<T, TComps...>();
			auto arrItr = getItem.begin();
			auto endItr = getItem.end();

			while (arrItr != endItr) {
				if (
					// tupleを展開して格納
					std::apply([&](auto... args)
						{
							return func(args...);
						}, *arrItr)
					) {
					this->erase(arrItr.GetIndex());

					// 終端の距離を1縮小
					--endItr;
				}
				else {
					++arrItr;
				}
			}
		}

		static void *GetItemPtr(void *begin, size_t size, uint32_t index);

	private:
		uint32_t size_{};
		uint32_t entitySize_{};

		MultiArray *const parent_ = nullptr;
		const Archetype *const archetype_;

		std::unique_ptr<void, ChunkDeleter> memoryPtr_;

		//std::unordered_map<ClassData, void *> componentAddress_;
		std::unordered_map<ClassDataManager::KeyType, void *> componentAddress_;
	};

	class MultiArray {
	public:
		MultiArray(const Archetype &archetype) : archetype_(archetype) { }
		~MultiArray() = default;
		using MultiChunkClass = std::vector<std::unique_ptr<MultiChunk>>;

		template<typename... TComps>
		class MultiCompArray {
		public:
			MultiCompArray() = default;
			MultiCompArray(MultiChunkClass *const multiChunk, const uint32_t capacity) :pMultiChunk_(multiChunk), capacity_(capacity) {}
			~MultiCompArray() = default;

			class iterator {
			public:
				iterator() = default;
				iterator(const iterator &) = default;
				iterator(MultiChunkClass *const multiChunk, MultiChunkClass::iterator chunkItr, uint32_t index, const uint32_t capacity) :pMultiChunk_(multiChunk), chunkItr_(chunkItr), capacity_(capacity) {
					if (chunkItr_ != pMultiChunk_->end()) {
						compArray_ = (*chunkItr_)->get<TComps...>();
						compArrayItr_ = compArray_.begin();
						compArrayItr_ += index;
					}
				}
				~iterator() = default;

				iterator &operator++();

				iterator operator[](const size_t index) const;

				std::tuple<ECS::Entity *, TComps *...> operator *() {
					return *(this->compArrayItr_);
				}

				std::tuple<TComps *...> operator->() {
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

				size_t GetTotalIndex() const {
					size_t result = std::distance(pMultiChunk_->begin(), chunkItr_);
					result += compArrayItr_.GetIndex();
					return result;
				}

			private:
				uint32_t capacity_;
				MultiChunkClass *pMultiChunk_;
				MultiChunkClass::iterator chunkItr_;
				ComponetArray<TComps...> compArray_;
				ComponetArray<TComps...>::iterator compArrayItr_{};
			};

			iterator begin();
			iterator end();

		private:
			MultiChunkClass *pMultiChunk_;
			uint32_t capacity_;
		};


		const Archetype &GetArchetype() const { return archetype_; }

		MultiChunk *AddChunk();

		auto &GetChunk() { return multiChunk_; }

		template<typename T, typename...TComps>
		void erase_if(const std::function <bool(ECS::Entity *, T *, TComps *...)> &func) {
			Archetype archetype{};
			archetype.AddClassData<T, TComps...>();
			// もし、対応した型があったら実行
			if (archetype <= this->archetype_) {
				for (auto &chunk : multiChunk_) {
					chunk->erase_if(func);
				}
			}
			this->Normalize();
		}

		/// @brief 末尾に要素を追加
		/// @return 追加された要素のindex
		size_t push_back();

		/// @brief 末尾に要素を追加
		/// @return 追加された要素のindex
		size_t push_back(const ECS::Prefab &prefab);

		template<SoLib::IsNotPointer T, SoLib::IsNotPointer... TComps>
		std::tuple<T *, TComps *...> GetItem(size_t totalIndex);

		template<SoLib::IsNotPointer T>
		T &GetOneItem(size_t totalIndex);

		/// @brief 穴抜けの部分を埋める
		void Normalize();

		/// @brief データの破棄
		/// @param totalIndex 破棄するindex
		void erase(const size_t totalIndex);

		template<typename T, typename... TComps>
		MultiCompArray<T, TComps...>::iterator erase(typename const MultiCompArray<T, TComps...>::iterator &arrItr) {
			//auto result = arrItr;

			std::size_t totalIndex = arrItr.GetTotalIndex();
			this->erase(totalIndex);


			return this->get<T, TComps...>().begin()[totalIndex];
		}

		bool empty() const { return this->multiChunk_.empty() || this->multiChunk_.back()->empty(); }

		size_t size() const;

		void swap(const size_t totalIndexF, const size_t totalIndexS);

		template<typename T, typename... TComps>
		MultiCompArray<T, TComps...> get();

		template<typename T, typename... TComps>
		std::tuple<T *, TComps *...> create_back();

		template<typename... TComps>
		size_t emplace_back();

	private:
		Archetype archetype_;
		MultiChunkClass multiChunk_;
	};


#pragma region Func

	template<typename ...TComps>
	inline ComponetArray<TComps...> MultiChunk::get() {
		ComponetArray<TComps...> result;
		result.componentAddress_ = std::make_shared<std::tuple<TComps *...>>();
		*result.componentAddress_ = std::make_tuple(reinterpret_cast<TComps *>(componentAddress_.at({ typeid(TComps) }))...);

		result.multiChunk_ = this;

		result.size_ = size_;
		result.entitySize_ = archetype_->GetTotalSize();

		return result;
	}

	template<typename... T>
	inline std::tuple<T *...> MultiChunk::GetItem(const uint32_t index) {

#ifdef _DEBUG

		assert(size_ > index);

#endif // _DEBUG
		//size_t totalSize = archetype_->GetTotalSize();

		return std::make_tuple(
			(

				[this, index]()
				{
					auto it = this->componentAddress_.find({ typeid(T) });
					return (it == this->componentAddress_.end() ? nullptr :
						reinterpret_cast<T *>(static_cast<MultiChunk::memoryType *>(it->second) + index * entitySize_));
				}()
					)...);


	}

	template<typename T>
	inline T &MultiChunk::GetOneItem(const uint32_t index) {

#ifdef _DEBUG

		assert(size_ > index);

#endif // _DEBUG
		size_t totalSize = archetype_->GetTotalSize();

		return *(reinterpret_cast<T *const>(static_cast<MultiChunk::memoryType *>(this->componentAddress_.at(typeid(T)).first) + index * totalSize));

	}


	template<SoLib::IsNotPointer T, SoLib::IsNotPointer... TComps>
	std::tuple<T *, TComps *...> MultiArray::GetItem(size_t totalIndex) {
		const auto capacity = archetype_.GetChunkCapacity();

		return multiChunk_[totalIndex / capacity]->GetItem<T, TComps...>(static_cast<uint32_t>(totalIndex % capacity));

	}


	template<SoLib::IsNotPointer T>
	T &MultiArray::GetOneItem(size_t totalIndex) {
		const auto capacity = archetype_.GetChunkCapacity();

		return multiChunk_[totalIndex / capacity]->GetOneItem<T>(static_cast<uint32_t>(totalIndex % capacity));

	}


#pragma endregion


	template<typename ...TComps>
	inline ComponetArray<TComps...>::iterator::iterator(ComponetArray *const compArray, uint32_t index) {
		componentAddress_ = compArray->componentAddress_;
		index_ = index;
		entitySize_ = compArray->entitySize_;
		multiChunk_ = compArray->multiChunk_;
	}

	template<typename ...TComps>
	inline  std::tuple<ECS::Entity *, TComps *...> ComponetArray<TComps...>::iterator::operator*() {
		//std::tuple<Ts*...> result = componentAddress_;

		//return std::make_tuple((Ts *const)((char *)std::get<Is>(t) + sizeof(Ts))...);
		//return this->IncrementAddress<Ts...>(*this->componentAddress_, index_);
		return std::make_tuple(static_cast<ECS::Entity *>(multiChunk_->GetEntityPtr(index_)), (reinterpret_cast<TComps *>(multiChunk_->GetItemPtr(typeid(TComps), index_))) ...);
	}

	template<typename ...TComps>
	inline MultiArray::MultiCompArray<TComps...>::iterator MultiArray::MultiCompArray<TComps...>::begin() {
		return iterator(this->pMultiChunk_, this->pMultiChunk_->begin(), 0u, capacity_);
	}

	template<typename ...TComps>
	inline MultiArray::MultiCompArray<TComps...>::iterator MultiArray::MultiCompArray<TComps...>::end() {

		return iterator(this->pMultiChunk_, this->pMultiChunk_->end(), 0u, capacity_);
	}
	template<typename ...TComps>
	inline MultiArray::MultiCompArray<TComps...>::iterator &MultiArray::MultiCompArray<TComps...>::iterator::operator++() {

		// 内部イテレータを加算
		++compArrayItr_;
		// もし、末尾に到達していたらチャンクを加算
		while (compArrayItr_ == compArray_.end()) {
			++chunkItr_;

			// チャンクが終端以外は内部データを更新
			if (chunkItr_ != pMultiChunk_->end()) {
				compArray_ = (*chunkItr_)->get<TComps...>();
				compArrayItr_ = compArray_.begin();
			}
			else {
				compArray_ = {};
				compArrayItr_ = {};
				break;
			}

		}

		return *this;
	}

	template<typename ...TComps>
	inline MultiArray::MultiCompArray<TComps...>::iterator MultiArray::MultiCompArray<TComps...>::iterator::operator[](const size_t index) const {

		MultiArray::MultiCompArray<TComps...>::iterator result = *this;

		// 内部イテレータを加算
		result.compArrayItr_ += static_cast<uint32_t>(index);
		// もし、末尾に到達していたらチャンクを加算
		while (result.compArrayItr_.GetIndex() >= result.capacity_) {
			size_t diff = result.compArrayItr_.GetIndex() - result.capacity_;
			result.chunkItr_ = result.chunkItr_ + (result.compArrayItr_.GetIndex() / result.capacity_);
			uint32_t innerDiff = static_cast<uint32_t>(diff % result.capacity_);

			// チャンクが終端以外は内部データを更新
			if (result.chunkItr_ != result.pMultiChunk_->end()) {
				result.compArray_ = (*result.chunkItr_)->get<TComps...>();
				result.compArrayItr_ = result.compArray_.begin() + innerDiff;

			}
			else {
				result.compArray_ = {};
				result.compArrayItr_ = {};
			}

		}

		return result;
	}

	template<typename T, typename... TComps>
	inline MultiArray::MultiCompArray<T, TComps...> MultiArray::get() {
		MultiArray::MultiCompArray<T, TComps...> result{ &this->multiChunk_, static_cast<uint32_t>(archetype_.GetChunkCapacity()) };

		return result;
	}

	template<typename T, typename ...TComps>
	inline std::tuple<T *, TComps *...> MultiArray::create_back() {
		size_t totalIndex = this->push_back();

		// 可変長テンプレートを使ったtemplateGetItem関数に置き換える
		return multiChunk_.back()->GetItem<T, TComps...>(static_cast<uint32_t>(totalIndex % archetype_.GetChunkCapacity()));

		//return std::tuple<T, Ts...>();
	}

	template<typename ...TComps>
	inline size_t MultiArray::emplace_back() {

		size_t target = push_back();

		const auto chunk = multiChunk_.back().get();
		uint32_t index = static_cast<uint32_t>(target % archetype_.GetChunkCapacity());

		(new (chunk->GetItemPtr(typeid(TComps), index)) TComps{}, ...);

		return target;
	}

}