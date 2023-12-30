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



	private:
		std::unordered_map<std::type_index, std::unique_ptr<ECS::IComponent>> componentData_;
		Archetype archetype_;
	};

}