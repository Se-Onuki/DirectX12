#pragma once
#include <typeindex>
#include <stdint.h>
#include <list>
#include "Entity/Entity.hpp"
#include "../Utils/SoLib/SoLib.h"
#include <unordered_set>
#include <set>
#include "ClassData.h"
#include "Component/ComponentRegistry.h"

class Archetype {
public:

	ECS::ComponentRegistry::ComponentFlag compFlag_;

	static constexpr size_t OneChunkCapacity = 16u * 1024u;

	Archetype() = default;

	template<typename T, typename... TComps>
	void AddClassData() {
		compFlag_.AddComp<T, TComps...>();

		totalSize_ += sizeof(T);
		((totalSize_ += sizeof(TComps)),...);

		chunkCapacity_ = CalcCapacity();
	}
	bool operator==(const Archetype &other) const { return other.compFlag_ == compFlag_; }

	bool operator<=(const Archetype &other) const {
		return (compFlag_.Get() & other.compFlag_.Get()) == compFlag_.Get();
	}

	uint32_t GetTotalSize() const { return totalSize_; }
	uint32_t GetChunkCapacity() const { return chunkCapacity_; }

private:

	uint32_t CalcCapacity() const { return OneChunkCapacity / totalSize_; }

	uint32_t totalSize_ = sizeof(ECS::Entity);
	uint32_t chunkCapacity_;

	template<typename... TComps>
	void InnerAddClassData()
	{
		ClassDataManager *const classDataManager = ClassDataManager::GetInstance();
		totalSize_ = sizeof(ECS::Entity) + (classDataManager->AddClass<TComps>()->size_ + ...);
		chunkCapacity_ = OneChunkCapacity / totalSize_;
	}
};



namespace std {
	template<>
	struct hash<Archetype> {
		std::size_t operator()(const Archetype &obj) const {
			return obj.compFlag_.Get().to_ullong();
		}
	};
}