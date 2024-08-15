#include "Chunk.h"

namespace ECS {
	Chunk::Chunk(const Archetype &archetype)
	{
		// 初期化する
		Init(archetype);

	}
	void Chunk::Init(const Archetype &archetype)
	{
		const ECS::ComponentRegistry *registry = ECS::ComponentRegistry::GetInstance();

		// アーキタイプを転送
		archetype_ = archetype;

		// アーキタイプに保存された型の一覧
		const auto &compIndexList = archetype_.compFlag_.GetIndexList();
		// 型の数だけメモリを確保する
		componentDatas_.reserve(archetype_.compFlag_.Get().count());

		// エンティティストレージの確保
		storage_ = EntityArrayStorage{ archetype_.GetChunkCapacity() };
		// エンティティの数
		const uint32_t entCount = archetype_.GetChunkCapacity();

		// アーキタイプに保存された型からメモリ配置を行う
		for (const uint32_t i : compIndexList) {
			// 型の情報を取得する
			const auto &typeData = registry->typeDatas_[i];

			// 型から情報を取得して､ポインタを保存する
			componentDatas_[i] = ComponentData{ typeData.typeSize_, entCount };
		}

		AddGroups();

	}

	void Chunk::AddGroups(const uint32_t count)
	{
		// エンティティストレージの確保
		const auto groups = storage_.AddGroup(count);

		// アーキタイプに保存された型の一覧
		const auto &compIndexList = archetype_.compFlag_.GetIndexList();
		const ECS::ComponentRegistry *registry = ECS::ComponentRegistry::GetInstance();

		// エンティティの数
		const uint32_t entCount = archetype_.GetChunkCapacity();

		uint32_t componentOffset = 0u;

		// アーキタイプに保存された型からメモリ配置を行う
		for (const uint32_t compId : compIndexList) {
			// 型の情報を取得する
			const auto &typeData = registry->typeDatas_[compId];
			// 型から情報を取得して､ポインタを保存する
			auto &compData = componentDatas_[compId];

			// 指定された数だけ構築する
			for (uint32_t i = 0; i < count; i++) {

				// 始点のメモリ
				std::byte *memTarget = groups[i].second->data();

				// 配列の追加
				compData.AddArray(memTarget + componentOffset);
			}
			// 型のサイズ分加算する
			componentOffset += typeData.typeSize_ * entCount;

		}

	}

}