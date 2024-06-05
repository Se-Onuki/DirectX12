#include "LevelImporter.h"
#include "../../ECS/Entity/EntityManager.hpp"

bool SolEngine::LevelImporter::Import(const LevelData *const levelData, World *const world) const
{
	// 正しくデータにアクセスできるか
	if (not world or not levelData) { return false; }

	// 正しくデータにアクセスできた場合

	// モデルは全てBOXであるとする
	Model *const defaultModel = ModelManager::GetInstance()->GetModel("Block");

	// エンティティマネージャを取得
	auto *const entityManager = world->GetEntityManager();

	//Archetype archeType

	// すべてのentityを走査する
	for (const auto &object : levelData->objectList_) {

		ECS::Prefab prefab;
		prefab += ECS::ModelComp{ .model_ = defaultModel };
		prefab += ECS::PositionComp{ .position_ = object.transform_.translate_ };
		prefab += ECS::RotateComp{ .rotate_ = object.transform_.rotate_ };
		prefab += ECS::ScaleComp{ .scale_ = object.transform_.scale_ };
		prefab += ECS::TransformMatComp{};

		// エンティティを追加
		entityManager->CreateEntity(prefab);

	}

	return true;

}
