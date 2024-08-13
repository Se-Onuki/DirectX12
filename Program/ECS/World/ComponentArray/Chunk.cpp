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

		// メモリ確保
		memData_.push_back(std::make_unique<std::byte[]>(Archetype::kOneChunkCapacity));

		// アーキタイプに保存された型の一覧
		const auto &compIndexList = archetype_.compFlag_.GetIndexList();
		// 型の数だけメモリを確保する
		compArrays_.reserve(compIndexList.size());

		// エンティティの数
		const uint32_t entCount = archetype_.GetChunkCapacity();

		// 始点のメモリを加算する
		std::byte *memTarget = memData_.begin()->get();

		// Entityの分メモリを加算する
		memTarget += sizeof(ECS::Entity) * entCount;

		// アーキタイプに保存された型からメモリ配置を行う
		for (const uint32_t i : compIndexList) {
			// 型の情報を取得する
			const auto &typeData = registry->typeDatas_[i];
			// 型から情報を取得して､ポインタを保存する
			compArrays_[i].push_back(ComponentArray(memTarget, typeData.typeSize_, entCount));
			// 型のサイズ分加算する
			memTarget += typeData.typeSize_ * entCount;
		}


	}
}