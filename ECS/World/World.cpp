#include "World.hpp"
#include "../Entity/EntityManager.hpp"

World::World() { entityManager_ = std::make_unique<ECS::EntityManager>(this); }
