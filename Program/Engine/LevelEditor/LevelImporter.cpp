#include "LevelImporter.h"
#include "../../ECS/Entity/EntityManager.hpp"
namespace SolEngine {
	bool LevelImporter::Import(const ResourceObjectManager<LevelData>::Handle levelData, World *const world) const
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
			prefab += ECS::CreateByLevelData{ .handle_ = levelData };

			// エンティティを追加
			entityManager->CreateEntity(prefab);

		}

		return true;

	}

	bool LevelImporter::Export(ResourceObjectManager<LevelData>::Handle levelData, const World *const world) const
	{
		// 正しくデータにアクセスできるか
		if (not world or not levelData) { return false; }

		// 正しくデータにアクセスできた場合

		// レベルデータを破棄
		levelData->objectList_.clear();

		for (const auto [entity, scale, rotate, position, createByData] : world->view<ECS::ScaleComp, ECS::RotateComp, ECS::PositionComp, ECS::CreateByLevelData>()) {

			// 保存先のデータを決める
			LevelData::ObjectData &objectData = levelData->objectList_.emplace_back();

			// 座標を設定する
			objectData.transform_ = { .scale_ = *scale, .rotate_ = *rotate, .translate_ = *position };

		}

		return true;
	}
}