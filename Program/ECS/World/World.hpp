#pragma once
#include "../Chunk/Chunk.hpp"
#include <functional>
#include <list>
#include "../MultiArray.h"
#include "NewWorld.h"
namespace ECS {
	class EntityManager;

	template<typename... TComps>
	class View {
	public:
		View() = default;

		class iterator {
		public:

			using value_type = std::tuple<ECS::Entity *, TComps *const...>;
			using iterator_concept = std::bidirectional_iterator_tag;
			using iterator_category = std::forward_iterator_tag;
			using difference_type = int32_t;

			//using difference_type = std::tuple<ECS::Entity *, TComps *const...>;
			//using value_type = std::tuple<ECS::Entity *, TComps *const...>;
			//using iterator_concept = std::forward_iterator_tag;

			iterator() = default;
			iterator(View *const view) { mArrayList_ = view->mArrayList_; }
			iterator(const iterator &) = default;
			iterator &operator=(const iterator &) = default;
			iterator(iterator &&) = default;
			iterator &operator=(iterator &&) = default;

			std::shared_ptr<std::list<ECS::MultiArray *>> mArrayList_;
			std::list<ECS::MultiArray *>::iterator mArrayListItr_;

			ECS::MultiArray::MultiCompArray<TComps...> mDataArray_;
			ECS::MultiArray::MultiCompArray<TComps...>::iterator mDataArrayItr_;

			std::tuple<ECS::Entity *, TComps *const...> operator*() {
				return *mDataArrayItr_;
			}

			std::tuple<ECS::Entity *, TComps *const...> operator->() {
				return *mDataArrayItr_;
			}

			iterator &operator++() {

				++mDataArrayItr_;

				while (not (mDataArrayItr_ != mDataArray_.end())) {
					++mArrayListItr_;

					if (mArrayListItr_ != mArrayList_->end()) {
						mDataArray_ = (*mArrayListItr_)->get<TComps...>();
						mDataArrayItr_ = mDataArray_.begin();
					}
					else {
						mDataArray_ = {};
						mDataArrayItr_ = {};
						break;
					}

				}


				return *this;
			}

			iterator operator++(int) &{
				iterator itr = *this;
				return ++itr;
			}

			bool operator!=(const iterator &other)const {
				if (/*this->mArrayList_ != other.mArrayList_ ||*/ this->mArrayListItr_ != other.mArrayListItr_) {
					return true;
				}
				if (/*this->mDataArray_ == ECS::MultiArray::MultiCompArray<Ts...>{} ||*/ not (this->mDataArrayItr_ != decltype(this->mDataArrayItr_){})) {
					return false;
				}
				if (/*this->mDataArray_ != other.mDataArray_ ||*/ this->mDataArrayItr_ != other.mDataArrayItr_) {
					return true;
				}
				return false;
			}

			bool operator==(const iterator &other)const {
				return not(*this != other);
			}


		};

		iterator begin() {
			iterator result{ this };
			result.mArrayListItr_ = result.mArrayList_->begin();
			if (result.mArrayList_->size()) {
				result.mDataArray_ = (*result.mArrayListItr_)->get<TComps...>();
				result.mDataArrayItr_ = result.mDataArray_.begin();
			}
			return result;
		}

		iterator end() {
			iterator result{ this };
			result.mArrayListItr_ = result.mArrayList_->end();

			return result;
		}



		//private:

		std::shared_ptr<std::list<ECS::MultiArray *>> mArrayList_;
	};

	template<typename... TComps>
	class ConstView {
	public:
		ConstView() = default;

		class iterator {
		public:
			iterator(ConstView *const view) { mArrayList_ = view->mArrayList_; }

			//private:
			std::shared_ptr<std::list<ECS::MultiArray *>> mArrayList_;
			std::list<ECS::MultiArray *>::iterator mArrayListItr_;

			ECS::MultiArray::MultiCompArray<TComps...> mDataArray_;
			ECS::MultiArray::MultiCompArray<TComps...>::iterator mDataArrayItr_;

			std::tuple<const ECS::Entity *const, const TComps *const...> operator*() {
				return *mDataArrayItr_;
			}

			iterator &operator++() {

				++mDataArrayItr_;

				while (not (mDataArrayItr_ != mDataArray_.end())) {
					++mArrayListItr_;

					if (mArrayListItr_ != mArrayList_->end()) {
						mDataArray_ = (*mArrayListItr_)->get<TComps...>();
						mDataArrayItr_ = mDataArray_.begin();
					}
					else {
						mDataArray_ = {};
						mDataArrayItr_ = {};
						break;
					}

				}


				return *this;
			}

			bool operator!=(const iterator &other) {
				if (/*this->mArrayList_ != other.mArrayList_ ||*/ this->mArrayListItr_ != other.mArrayListItr_) {
					return true;
				}
				if (/*this->mDataArray_ == ECS::MultiArray::MultiCompArray<Ts...>{} ||*/ not (this->mDataArrayItr_ != decltype(this->mDataArrayItr_){})) {
					return false;
				}
				if (/*this->mDataArray_ != other.mDataArray_ ||*/ this->mDataArrayItr_ != other.mDataArrayItr_) {
					return true;
				}
				return false;
			}

			bool operator==(const iterator &other) {
				return not (*this != other);
			}


		};

		iterator begin() {
			iterator result{ this };
			result.mArrayListItr_ = result.mArrayList_->begin();
			if (result.mArrayList_->size()) {
				result.mDataArray_ = (*result.mArrayListItr_)->get<TComps...>();
				result.mDataArrayItr_ = result.mDataArray_.begin();
			}
			return result;
		}

		iterator end() {
			iterator result{ this };
			result.mArrayListItr_ = result.mArrayList_->end();

			return result;
		}


		std::shared_ptr<std::list<ECS::MultiArray *>> mArrayList_;
	};

}

class SystemBase;
class World {

public:
	World();
	~World() = default;

	ECS::EntityManager *GetEntityManager() { return entityManager_.get(); }

	bool IsHasChank(const Archetype &archetype) {
		return chunkList_.contains(archetype);
	}

	ECS::MultiArray *CreateChunk(const Archetype &archetype) {
		chunkList_[archetype] = std::make_unique<ECS::MultiArray>(archetype);
		return chunkList_.at(archetype).get();
	}

	ECS::MultiArray *GetChunk(const Archetype &archetype) {
		const auto &item = chunkList_.find(archetype);
		if (item == chunkList_.end()) {
			return nullptr;
		}
		return item->second.get();
	}

	size_t size() {
		size_t result{};

		for (const auto &mArray : chunkList_) {
			result += mArray.second->size();
		}

		return result;
	}

	template<typename T, typename... TComps>
	ECS::View<T, TComps...> view() {
		ECS::View<T, TComps...> result;
		result.mArrayList_ = std::make_shared<std::list<ECS::MultiArray *>>();

		Archetype checkArche;
		checkArche.AddClassData<T, TComps...>();

		for (const auto &[archetype, mArray] : chunkList_) {
			if (checkArche <= archetype) {
				if (not mArray->empty()) {
					result.mArrayList_->push_back(mArray.get());
				}
			}
		}
		return result;
	}

	template<typename T, typename... TComps>
		requires (SoLib::IsConst<T> && ... && SoLib::IsConst<TComps>)
	ECS::ConstView<T, TComps...> view() {
		ECS::ConstView<T, TComps...> result;
		result.mArrayList_ = std::make_shared<std::list<ECS::MultiArray *>>();

		// アーキタイプを検出する
		Archetype checkArche;
		checkArche.AddClassData<T, TComps...>();

		for (const auto &[archetype, mArray] : chunkList_) {
			if (checkArche <= archetype) {
				if (not mArray->empty()) {
					result.mArrayList_->push_back(mArray.get());
				}
			}
		}
		return result;
	}
	template<typename T, typename... TComps>
	ECS::ConstView<T, TComps...> view() const {
		ECS::ConstView<T, TComps...> result;
		result.mArrayList_ = std::make_shared<std::list<ECS::MultiArray *>>();

		Archetype checkArche;
		checkArche.AddClassData<T, TComps...>();

		for (const auto &[archetype, mArray] : chunkList_) {
			if (checkArche <= archetype) {
				if (not mArray->empty()) {
					result.mArrayList_->push_back(mArray.get());
				}
			}
		}
		return result;
	}

	template<typename T, typename...TComps>
	void erase_if(const std::function <bool(T *, TComps *...)> &func) {
		for (const auto &[archetype, chunk] : chunkList_) {
			chunk->erase_if(func);
		}
		// 不要なデータを破棄
		//std::erase_if(chunkList_, [](const auto &pair)
		//	{
		//		// データが保存されていないコンテナを破棄する
		//		return not pair.second->size();
		//	}
		//);

	}

private:
	std::unordered_map<Archetype, std::unique_ptr<ECS::MultiArray>> chunkList_ = {};
	std::unique_ptr<ECS::EntityManager> entityManager_;
};