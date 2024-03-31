#pragma once
#include <typeindex>
#include <stdint.h>
#include <list>
#include "Entity/Entity.hpp"
#include "../Utils/SoLib/SoLib.h"
#include <unordered_set>
#include <set>
#include "ClassData.h"

class Archetype {
public:
	std::set<ClassDataManager::KeyType> data_;

	static constexpr size_t OneChunkCapacity = 16u * 1024u;

	Archetype() = default;

	template<typename T, typename... Ts>
	void AddClassData() {
		//// 再帰前の処理
		//{
		//	// クラスデータを追加する
		//	ClassDataManager::GetInstance()->AddClass<T>();
		//	// 型情報を追加
		//	data_.insert({ typeid(T) });
		//}
		//if constexpr (sizeof...(Ts) > 0) {
		//	AddClassData<Ts...>();
		//}
		//else {
		//	totalSize_ = CalcTotalSize();
		//	chunkCapacity_ = CalcCapacity();
		//}

		InnerAddClassData<T, Ts...>();
	}
	bool operator==(const Archetype &other) const { return other.data_ == data_; }

	bool operator<=(const Archetype &other) const {
		if (data_.size() > other.data_.size()) {
			return false;
		}
		for (const auto &element : data_) {
			// 保存されてなかったら終わり
			if (not other.data_.count(element)) {
				return false;
			}
		}
		return true;
	}

	size_t GetTotalSize() const { return totalSize_; }
	size_t GetChunkCapacity() const { return chunkCapacity_; }

private:

	size_t CalcTotalSize() const {
		size_t result = 0u;
		result += sizeof(ECS::Entity);

		for (auto &item : data_) {
			result += item->size_;
		}

		return result;
	}

	size_t CalcCapacity() const {
		return OneChunkCapacity / this->CalcTotalSize();
	}

	size_t totalSize_;
	size_t chunkCapacity_;

	template<typename... Ts>
	void InnerAddClassData()
	{
		ClassDataManager *const classDataManager = ClassDataManager::GetInstance();
		(data_.insert({ typeid(Ts) }), ...);
		totalSize_ = (classDataManager->AddClass<Ts>()->size_ + ...);
		chunkCapacity_ = OneChunkCapacity / totalSize_;
	}
};



namespace std {
	template<>
	struct hash<Archetype> {
		std::size_t operator()(const Archetype &obj) const {
			std::string typeNames;

			for (const auto &type : obj.data_) {
				typeNames += type.typeIndex_.name();
			}
			return std::hash<std::string>{}(typeNames);

		}
	};
}