/// @file NewWorld.cpp
/// @brief ECSのワールド
/// @author ONUKI seiya
#include "NewWorld.h"

namespace ECS {

	const EntityClass &World::CreateEntity(const Archetype &archetype)
	{
		auto chunk = CreateOrGetChunk(archetype);

		uint32_t index = chunk->emplace_back();

		return chunk->GetEntity(index);
	}

	const EntityClass &World::CreateEntity(const Prefab &prefab)
	{
		// エンティティを構築する
		auto &&entity = CreateEntity(prefab.GetArchetype());

		// コンポーネントの型情報の保存先
		const auto *compRegistry = ECS::ComponentRegistry::GetInstance();
		// コンポーネントの元データのマップ
		const auto &prefabComp = prefab.GetComponentMap();

		// プレハブに保存されたコンポーネントのid
		for (uint32_t i : prefab.GetArchetype().required_.GetIndexList()) {
			// もしコンポーネントを持っていなかったら
			// if (not prefab.GetArchetype().required_.Get()[i]) [[unlikely]] { continue; }

			// 型情報の取得
			const auto &typeKey = compRegistry->typeDatas_[i];

			// 書き込み先のポインタ
			auto ptr = entity.chunk_->GetComp(i, entity.totalIndex_);
			// データのコピー処理
			std::memcpy(ptr, prefabComp.at(typeKey.typeIndex_).get(), typeKey.typeSize_);
		}

		return entity;

	}

	EntityList<false> World::CreateEntity(const Archetype &archetype, uint32_t count)
	{
		// 生成先のチャンク
		auto chunk = CreateOrGetChunk(archetype);

		// 生成したエンティティの保存領域
		const auto &&[begin, end] = chunk->emplace_back(count);
		// 生成されたエンティティの集合
		return EntityList<false>{ chunk, begin, end };

	}

	EntityList<false> World::CreateEntity(const Prefab &prefab, uint32_t count)
	{
		// エンティティの生成
		auto &&entityList = CreateEntity(prefab.GetArchetype(), count);
		// コンポーネントのデータ
		const auto *compRegistry = ECS::ComponentRegistry::GetInstance();
		// プレハブ
		const auto &prefabComp = prefab.GetComponentMap();

		// コンポーネントのid
		for (uint32_t i : prefab.GetArchetype().required_.GetIndexList()) {
			// if (not prefab.GetArchetype().required_.Get()[i]) { continue; }

			// 型情報
			const auto &typeKey = compRegistry->typeDatas_[i];

			// ソースデータ
			auto *const src = prefabComp.at(typeKey.typeIndex_).get();
			// 書き込み先のコンポーネントの配列
			auto component = entityList.GetChunk()->GetComponent(i);

			// エンティティの場所
			for (const auto entity : entityList.ItrRange()) {
				// 配列から書き込み先のアドレスを取得する
				auto ptr = &component->at(entity);
				// 書き込み
				std::memcpy(ptr, src, typeKey.typeSize_);
			}
		}

		return entityList;
	}

}
