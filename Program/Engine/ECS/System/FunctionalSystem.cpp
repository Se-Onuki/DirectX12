/// @file FunctionalSystem.cpp
/// @brief 関数式のシステムの実行クラス
/// @author ONUKI seiya
#include "FunctionalSystem.h"
#include "../World/ComponentArray/Chunk.h"
#include "../World/NewWorld.h"

namespace ECS {
	void SystemExecuter::Execute(const SystemData &system, const World *const world, Chunk *const chunk, const float deltaTime)
	{
		// コンポーネントの数
		const uint32_t compCount = static_cast<uint32_t>(system.keys_.size());
		// コンポーネントデータのソース
		std::vector<ComponentSpan *> sourceData{ compCount };
		std::transform(system.keys_.begin(), system.keys_.end(), sourceData.begin(), [&chunk](uint32_t compId) {return chunk->GetComponent(compId); });

		const auto &function = [world, &system, &sourceData, compCount, deltaTime](EntityClass &entity)
			{
				// コンポーネントアドレスへの配列
				std::vector<std::byte *> compData{ compCount };
				for (uint32_t i = 0; i < compCount; i++) {
					// ソースデータから､コンポーネントのアドレスを取得する
					compData[i] = &sourceData[i]->at(entity.totalIndex_);
				}
				// コンポーネントを格納したデータから実行する
				system.executor_(std::bit_cast<IJobEntity *>(compData.data()), world, deltaTime);
			};

		if (system.isSingleThreads_) {
			std::for_each(chunk->begin(), chunk->end(), function);
		}
		else {
			std::for_each(std::execution::par, chunk->begin(), chunk->end(), function);
		}
	}

	void SystemExecuter::Execute(World *world, float deltaTime)
	{
		for (const auto &system : systems_) {

			auto chunkList = world->GetAccessableChunk(system.archetype_, system.exclusions_);
			if (system.preExecuter_) { system.preExecuter_(world, deltaTime); }
			std::for_each(chunkList.begin(), chunkList.end(), [&](Chunk *chunk) { Execute(system, world, chunk, deltaTime); });
		}

	}

}
