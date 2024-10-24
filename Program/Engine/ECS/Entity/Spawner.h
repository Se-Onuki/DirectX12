#pragma once
#include "Prefab.h"
#include "EntityManager.hpp"
#include "../World/NewWorld.h"

namespace ECS {

	class Spawner {
	private:

		// 生成時に呼び出される関数
		using SpawnFunc = void(*)(std::vector<ECS::EntityClass> &entList);

		struct SpawnData {
			// 基礎となるプレハブ
			Prefab *basePrefab_;
			// 生成時に実行する処理
			SpawnFunc spawnFunc_;
		};

		std::list<std::pair<SpawnData, uint32_t>> spawnDatas_;

	public:
		Spawner() = default;
		~Spawner() = default;

		void clear() {
			spawnDatas_.clear();
		}

		void Execute(ECS::World *world) {
			for (auto &[spawnData, count] : spawnDatas_) {
				// 指定された個数分追加する
				auto entList = world->CreateEntity(*spawnData.basePrefab_, count);
				// もし生成関数があれば
				if (spawnData.spawnFunc_) {
					// 生成関数を実行する
					spawnData.spawnFunc_(entList);
				}
			}
		}

		/// @brief 生成するオブジェクトを追加
		/// @param prefab 追加するプレハブ
		/// @param spawnCount 追加する数
		/// @param spawnFunc 実行する関数
		void AddSpawner(Prefab *prefab, uint32_t spawnCount = 1u, SpawnFunc spawnFunc = nullptr) {
			spawnDatas_.push_back({ {prefab,spawnFunc},spawnCount });
		}

	};

}
