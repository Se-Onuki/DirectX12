#pragma once
#include <vector>
#include "../../Entity/Entity.hpp"
#include <memory>

namespace ECS {
	class EntityArrayStorage
	{
	public:
		EntityArrayStorage() = default;
		~EntityArrayStorage() = default;

	private:

		std::vector<std::pair<std::unique_ptr<Entity[]>, std::unique_ptr<std::byte[]>>> entityData_;

	};
}