#include "FunctionalSystem.h"
#include "../World/ComponentArray/Chunk.h"

namespace ECS {

	void SystemExecuter::Execute(Chunk *chunk)
	{
		for (const auto &system : systems_) {
			// コンポーネントの数
			const uint32_t compCount = static_cast<uint32_t>(system.first.size());
			// コンポーネントデータのソース
			std::vector<ComponentData *> sourceData{ compCount };
			std::transform(system.first.begin(), system.first.end(), sourceData.begin(), [&chunk](uint32_t compId) {return chunk->GetComponent(compId); });

			std::for_each(std::execution::par, chunk->begin(), chunk->end(), [&system, &sourceData,compCount](EntityClass& entity) {// コンポーネントアドレスへの配列
				std::vector<std::byte *> compData{ compCount };
				for (uint32_t i = 0; i < compCount; i++) {
					// ソースデータから､コンポーネントのアドレスを取得する
					compData[i] = &sourceData[i]->at(entity.totalIndex_);
				}

				// データを構築
				auto target = system.second(compData.data());

				// 処理の実行
				target->Execute(); 
				}
			);

			//for (uint32_t idx = 0; idx < chunk->size(); idx++) {

			//	// コンポーネントアドレスへの配列
			//	std::vector<std::byte *> compData{ compCount };
			//	for (uint32_t i = 0; i < compCount; i++) {
			//		// ソースデータから､コンポーネントのアドレスを取得する
			//		compData[i] = &sourceData[i]->at(idx);
			//	}

			//	// データを構築
			//	auto target = system.second(compData.data());

			//	// 処理の実行
			//	target->Execute();

			//}

		}
	}

}
