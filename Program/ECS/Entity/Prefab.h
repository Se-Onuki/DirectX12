#pragma once
#include "../Component/Component.hpp"
#include <memory>
#include <typeindex>
#include <unordered_map>
#include "../Archetype.h"

namespace ECS {

	class Prefab {
	public:
		Prefab() = default;
		~Prefab() = default;

		template<IsComponent T>
		T &AddComponent();

		const auto &GetComponentMap() const { return componentData_; }

		const Archetype &GetArchetype() const { return archetype_; }

		template<IsComponent T>
		Prefab &operator+= (const T &comp) {
			this->AddComponent<T>() = comp;
			return *this;
		}

	private:
		std::unordered_map<std::type_index, std::unique_ptr<ECS::IComponent>> componentData_;
		Archetype archetype_;
	};



	template<IsComponent T>
	inline T &Prefab::AddComponent() {
		// typeindexを作成
		const std::type_index type = typeid(T);
		// 既に保存されているか確認
		auto findItr = componentData_.find(type);
		// 存在したらそのまま返す
		if (findItr != componentData_.end()) { return *reinterpret_cast<T *>(findItr->second.get()); }
		// 存在しなかったら新しく追加
		componentData_[type] = std::make_unique<T>();
		// アーキタイプも追加
		archetype_.AddClassData<T>();
		// ポインタを返す
		return *reinterpret_cast<T *>(componentData_.at(type).get());
	}

}