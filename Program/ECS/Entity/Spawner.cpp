#include "Spawner.h"
#include "../World/NewWorld.h"

void ECS::Spawner::Execute(ECS::World *world)
{
	for (auto &[spawnData, count] : spawnDatas_) {
		// 指定された個数分追加する
		auto entList = world->CreateEntity(*spawnData.basePrefab_, count);
		// もし生成関数があれば
		if (spawnData.spawnFunc_) {
			// 生成関数を実行する
			//spawnData.spawnFunc_(entList, nullptr);
		}
	}
}
