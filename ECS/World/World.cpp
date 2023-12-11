#include "World.hpp"
#include "../Entity/EntityManager.hpp"

World::World() { entityManager_ = std::make_unique<ECS::EntityManager>(this); }

World::~World() {
	for (auto &element : chunkList_) {
		element.ChankDelete();
	}
}
