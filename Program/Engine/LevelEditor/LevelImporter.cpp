/// @file LevelImporter.cpp
/// @brief レベルの読み込みクラス
/// @author ONUKI seiya
#include "LevelImporter.h"
namespace SolEngine {

	bool LevelImporter::Import(const ResourceObjectManager<LevelData>::Handle levelData, ECS::World *const world) const
	{
		// 正しくデータにアクセスできるか
		if (not world or not levelData) { return false; }

		// 正しくデータにアクセスできた場合

		// 再帰的にロードを行う
		RecursiveLoad(levelData->objectList_, world);

		return true;

	}
	void LevelImporter::RecursiveLoad(const std::list<LevelData::ObjectData> &objectDataList, ECS::World *const world, [[maybe_unused]] ECS::EntityClass *parent) const
	{
		// すべてのentityを走査する
		for (const auto &object : objectDataList) {

			ECS::Prefab prefab;
			// マネージャからモデルを取得する
			ResourceObjectManager<ModelData>::Handle model = object.modelHandle_;

			// モデルデータを持っていた場合
			if (model) { // コンポーネントを追加する
				prefab += ECS::ModelComp{ .model_ = object.modelHandle_ };
			}
			{
				prefab += ECS::PositionComp{ .position_ = object.transform_.translate_ };
				prefab += ECS::QuaternionRotComp{ .quateRot_ = object.transform_.rotate_ };
				prefab += ECS::ScaleComp{ .scale_ = object.transform_.scale_ };
				prefab += ECS::TransformMatComp{};
			}
			prefab += ECS::CreateByLevelData{};

			// OBB型なら代入する
			if (auto ptr = std::get_if<OBB>(&object.collider_); ptr) {
				prefab += ECS::OBBCollisionComp{ .collision_ = *ptr };
			}

			// 親の情報があったら渡す
			/*if (parent) {
				prefab += ECS::Parent{ .parent_ = *parent };
			}*/

			// エンティティを追加
			ECS::EntityClass entity = world->CreateEntity(prefab);

			// 追加したオブジェクトの子供を作成する
			RecursiveLoad(object.children_, world, &entity);

		}
	}
}
