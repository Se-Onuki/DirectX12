#pragma once
#include <typeindex>
#include <stdint.h>
#include <list>
#include "Entity/Entity.hpp"
#include "../Utils/SoLib/SoLib.h"
#include <unordered_set>

class ClassData {
public:
	std::type_index typeInfo_ = typeid(void);
	size_t size_;

	void (*constructor_)(void *);

	template<SoLib::IsNotPointer T>
	static ClassData Create() {
		return { typeid(T), sizeof(T), [](void *ptr) { new(ptr) T{}; } };
	}

	bool operator==(const ClassData &other) const {
		return (typeInfo_ == other.typeInfo_ && size_ == other.size_);
	}

};


namespace std {
	template<>
	struct hash<ClassData> {
		std::size_t operator()(const ClassData &obj) const {
			// std::type_index のハッシュ値をそのまま返す
			return obj.typeInfo_.hash_code();
		}
	};
}

class Archetype {
public:
	std::unordered_set<ClassData> data_;

	static constexpr size_t OneChunkCapacity = 16u * 1024u;

	Archetype() = default;

	template<typename T, typename... Ts> void AddClassData() {
		data_.insert(ClassData::Create<T>());
		if constexpr (sizeof...(Ts) > 0) {
			AddClassData<Ts...>();
		}
		else {
			totalSize_ = CalcTotalSize();
			chunkCapacity_ = CalcCapacity();
		}
	}
	bool operator==(const Archetype &other) const { return other.data_ == data_; }

	bool operator<=(const Archetype &other) const {
		if (data_.size() > other.data_.size()) {
			return false;
		}
		for (const auto &element : data_) {
			if (std::find(other.data_.begin(), other.data_.end(), element) == other.data_.end()) {
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
			result += item.size_;
		}

		return result;
	}

	size_t CalcCapacity() const {
		return OneChunkCapacity / this->CalcTotalSize();
	}

	size_t totalSize_;
	size_t chunkCapacity_;
};



namespace std {
	template<>
	struct hash<Archetype> {
		std::size_t operator()(const Archetype &obj) const {
			std::string typeNames;

			for (const auto &type : obj.data_) {
				typeNames += type.typeInfo_.name();
			}
			return std::hash<std::string>{}(typeNames);

		}
	};
}