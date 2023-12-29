#pragma once
#include "../Chunk/Chunk.hpp"
#include <functional>
#include <list>
#include "../MultiArray.h"
namespace ECS {
	class EntityManager;

	template<typename... Ts>
	class View {
	public:
		View() = default;

		class iterator {
		public:
			iterator(View *const view) { mArrayList_ = view->mArrayList_; }

			//private:
			std::shared_ptr<std::list<ECS::MultiArray *>> mArrayList_;
			std::list<ECS::MultiArray *>::iterator mArrayListItr_;

			ECS::MultiArray::MultiCompArray<Ts...> mDataArray_;
			ECS::MultiArray::MultiCompArray<Ts...>::iterator mDataArrayItr_;

			std::tuple<Ts *const...> operator*() {
				return *mDataArrayItr_;
			}

			iterator &operator++() {

				++mDataArrayItr_;

				while (not (mDataArrayItr_ != mDataArray_.end())) {
					++mArrayListItr_;

					if (mArrayListItr_ != mArrayList_->end()) {
						mDataArray_ = (*mArrayListItr_)->get<Ts...>();
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


		};

		iterator begin() {
			iterator result{ this };
			result.mArrayListItr_ = result.mArrayList_->begin();
			if (result.mArrayList_->size()) {
				result.mDataArray_ = (*result.mArrayListItr_)->get<Ts...>();
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

}


class SystemBase;
class World {

public:
	World();
	~World() = default;

	ECS::EntityManager *GetEntityManager() { return entityManager_.get(); }

	ECS::MultiArray *CreateChunk(const Archetype &archetype) {
		chunkList_[archetype] = std::make_unique<ECS::MultiArray>(archetype);
		return chunkList_.at(archetype).get();
	}

	ECS::MultiArray *GetChunk(const Archetype &archetype) {
		auto item = chunkList_.find(archetype);
		if (item == chunkList_.end()) {
			return nullptr;
		}
		return item->second.get();
	}

	size_t Count() {
		size_t result{};

		for (const auto &mArray : chunkList_) {
			result += mArray.second->size();
		}

		return result;
	}

	/*template<typename T, typename... Ts>
	std::list<ECS::MultiArray *> view() {
		std::list<ECS::MultiArray *> result;

		Archetype checkArche;
		checkArche.AddClassData<T, Ts...>();

		for (const auto &[archetype, mArray] : chunkList_) {
			if (checkArche <= archetype) {
				result.push_back(mArray.get());
			}
		}
		return result;
	}*/

	template<typename T, typename... Ts>
	ECS::View<T, Ts...> view() {
		ECS::View<T, Ts...> result;
		result.mArrayList_ = std::make_shared<std::list<ECS::MultiArray *>>();

		Archetype checkArche;
		checkArche.AddClassData<T, Ts...>();

		for (const auto &[archetype, mArray] : chunkList_) {
			if (checkArche <= archetype) {
				if (not mArray->empty()) {
					result.mArrayList_->push_back(mArray.get());
				}
			}
		}
		return result;
	}

	template<typename T, typename...Ts>
	void erase_if(const std::function <bool(T *, Ts *...)> &func) {
		for (const auto &[archetype, chunk] : chunkList_) {
			chunk->erase_if(func);
		}
		// 不要なデータを破棄
		std::erase_if(chunkList_, [](const auto &pair)
			{
				// データが保存されていないコンテナを破棄する
				return not pair.second->size();
			}
		);

	}


	//template<typename T, typename...Ts> void ForEach(const std::function<void(T *, Ts*...)> &func);
	/*template<typename A, typename B> void ForEach(std::function<void(A &, B &)> func);
	template<typename A, typename B, typename C> void ForEach(std::function<void(A &, B &, C &)> func);*/

private:
	std::unordered_map<Archetype, std::unique_ptr<ECS::MultiArray>> chunkList_ = {};
	std::unique_ptr<ECS::EntityManager> entityManager_;
};
//
//template<typename T, typename...Ts> inline void World::ForEach(const std::function<void(T *, Ts*...)> &func) {
//	Archetype arType;
//	arType.AddClassData<T, Ts...>();
//	for (auto &[archetype, mArray] : chunkList_) {
//		if (not (arType <= archetype)) { continue; }
//		for (size_t i = 0; i < mArray->size(); i++) {
//
//			auto item = mArray->GetItem<T, Ts...>(i);
//			std::apply([&](auto... args)
//				{
//					return func(args...);
//				}, item);
//		}
//	}
//}
//
//template<typename A, typename B> inline void World::ForEach(std::function<void(A &, B &)> func) {
//	Archetype arType;
//	arType.AddClassData<A, B>();
//	for (Chunk &chunk : chunkList_) {
//		if (!(arType <= chunk.GetArchetype()))
//			continue;
//		A *dataA = chunk.GetArray<A>();
//		B *dataB = chunk.GetArray<B>();
//		for (uint32_t i = 0; i < chunk.entityCount_; i++) {
//			func(dataA[i], dataB[i]);
//		}
//	}
//}
//
//template<typename A, typename B, typename C>
//inline void World::ForEach(std::function<void(A &, B &, C &)> func) {
//	Archetype arType;
//	arType.AddClassData<A, B, C>();
//	for (Chunk &chunk : chunkList_) {
//		if (!(arType <= chunk.GetArchetype()))
//			continue;
//		A *dataA = chunk.GetArray<A>();
//		B *dataB = chunk.GetArray<B>();
//		C *dataC = chunk.GetArray<C>();
//		for (uint32_t i = 0; i < chunk.entityCount_; i++) {
//			func(dataA[i], dataB[i], dataC[i]);
//		}
//	}
//}
