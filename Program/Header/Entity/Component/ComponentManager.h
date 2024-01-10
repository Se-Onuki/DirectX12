#pragma once

#include <unordered_map>
#include <string>
#include <type_traits>

#include "../Entity.h"


class ComponentManager
{
	ComponentManager() = default;
	ComponentManager(const ComponentManager &) = delete;
	ComponentManager &operator=(const ComponentManager &) = delete;

	~ComponentManager() = default;
public:

	template<typename T>
	void Register();



private:
	std::unordered_map<std::string, IComponent *(*)(Entity *const object)> componentMap_;
};

template<typename T>
inline void ComponentManager::Register() {
	static_assert(std::is_base_of<IComponent, T>::value, "テンプレート型はIComponentクラスの派生クラスではありません");
	componentMap_[typeid(T).name] = T(Entity *const);
}
