#include "FunctionalSystem.h"
#include "../World/ComponentArray/Chunk.h"

namespace ECS {

	void SystemExecuter::Execute(Chunk *chunk)
	{
		auto &front = systems_.front();
		// コンポーネントの数
		const uint32_t compCount = static_cast<uint32_t>(front.first.size());
		// コンポーネントデータのソース
		std::vector<ComponentData *> source{ compCount };
		for (uint32_t i = 0; i < compCount; i++) {
			// コンポーネントのRangeを渡す
			source[i] = chunk->GetComponent(front.first[i]);
		}

		for (uint32_t idx = 0; idx < chunk->size(); idx++) {

			// コンポーネントアドレスへの配列
			std::vector<std::byte *> compData{ compCount };
			for (uint32_t i = 0; i < compCount; i++) {
				// ソースデータから､コンポーネントのアドレスを取得する
				compData[i] = &source[i]->at(idx);
			}

			// データを構築
			auto target = front.second(compData.data());	// 


			target->Execute();

		}

	}

}
