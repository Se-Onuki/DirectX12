#include "World.hpp"
#include "Header/Entity/EntityManager.hpp"

World::World() { entityManager_ = new EntityManager(this); }

World::~World() { 
	delete entityManager_;
	for (auto& element : chunkList_) {
		element.ChankDelete();
	}
}
