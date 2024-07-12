#include "LevelImporter.h"
#include "../../ECS/Entity/EntityManager.hpp"
namespace SolEngine {
	bool LevelImporter::Import(const ResourceObjectManager<LevelData>::Handle levelData, World *const world) const
	{
		// 正しくデータにアクセスできるか
		if (not world or not levelData) { return false; }

		// 正しくデータにアクセスできた場合

		//Archetype archeType
		RecursiveLoad(levelData->objectList_, world);

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

	void LevelImporter::RecursiveLoad(const std::list<LevelData::ObjectData> &objectDataList, World *const world, ECS::Entity *parent) const
	{

		/*auto *modelManager = ResourceObjectManager<ModelData>::GetInstance();
		auto *assimpManager = ResourceObjectManager<AssimpData>::GetInstance();*/

		//// モデルは初期値は"box.obj"であるとする
		//auto defaultModel = modelManager->Load({ assimpManager->Load({ "","box.obj" }) });

		// エンティティマネージャを取得
		auto *const entityManager = world->GetEntityManager();

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
				prefab += ECS::RotateComp{ .rotate_ = object.transform_.rotate_ };
				prefab += ECS::ScaleComp{ .scale_ = object.transform_.scale_ };
				prefab += ECS::TransformMatComp{};
			}
			prefab += ECS::CreateByLevelData{};

			// OBB型なら代入する
			if (auto ptr = std::get_if<OBB>(&object.collider_); ptr) {

				//// 当たり判定があったら入れる
				//if (object.collider_ != Vector3::zero) {

				prefab += ECS::OBBCollisionComp{ .collision_ = *ptr };
				//}
			}
			// 親の情報があったら渡す
			if (parent) {
				prefab += ECS::Parent{ .parent_ = *parent };
			}

			// エンティティを追加
			ECS::Entity entity = entityManager->CreateEntity(prefab).front();

			// 追加したオブジェクトの子供を作成する
			RecursiveLoad(object.children_, world, &entity);

		}
	}
}
