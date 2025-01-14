/// @file Spawner.h
/// @brief スポナー
/// @author ONUKI seiya
#pragma once
#include "Prefab.h"
#include "../World/NewWorld.h"

namespace ECS {

	class Spawner {
	private:

		// 生成時に呼び出される関数
		using SpawnFunc = std::function<void(const ECS::EntityList<false> &entList)>;

		struct SpawnData {
			// 基礎となるプレハブ
			const Prefab *basePrefab_;
			// 生成時に実行する処理
			SpawnFunc spawnFunc_;
		};

		std::list<std::pair<SpawnData, uint32_t>> spawnDatas_;

	public:
		Spawner() = default;
		~Spawner() = default;

		/// @brief スポナーをクリア
		void clear() {
			spawnDatas_.clear();
		}

		/// @brief スポナーを実行
		/// @param[out] world 実行するワールド
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
		/// @param[in] prefab 追加するプレハブ
		/// @param[in] spawnCount 追加する数
		/// @param[in] spawnFunc 実行する関数
		void AddSpawner(const Prefab *prefab, uint32_t spawnCount = 1u, SpawnFunc spawnFunc = nullptr) {
			spawnDatas_.push_back({ {prefab,spawnFunc},spawnCount });
		}

	};

}
