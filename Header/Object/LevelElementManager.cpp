#include "LevelElementManager.h"

void LevelElementManager::Init() {
}

void LevelElementManager::Update([[maybe_unused]] float deltaTime) {
}

void LevelElementManager::Draw([[maybe_unused]] const Camera3D &camera) const {
}

void LevelElementManager::AddBlock(const BaseTransform<false> &transform) {
	auto entity = std::make_unique<Entity>();

	entity->transform_.translate = transform.translate;


	block_.push_back(std::move(entity));
}

void LevelElementManager::AddBlock(Entity *const entity) {
	block_.emplace_back(entity);
}
