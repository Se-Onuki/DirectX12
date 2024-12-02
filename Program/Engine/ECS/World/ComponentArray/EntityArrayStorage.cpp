#include "EntityArrayStorage.h"
#include "Chunk.h"

std::span<ECS::EntityArrayStorage::EntityStorage> ECS::EntityArrayStorage::AddGroup(uint32_t count)
{
	// 追加前の個数
	const uint32_t beforeCount = static_cast<uint32_t>(entityStorage_.size());

	// データの確保
	entityStorage_.resize(beforeCount + count);

	// 追加したデータの先頭
	auto newBegin = entityStorage_.begin() + beforeCount;

	// 確保するメモリを初期化なしで割り当てる
	std::for_each(newBegin, entityStorage_.end(), [this](EntityStorage &item) {
		item = EntityStorage{
			std::make_unique_for_overwrite<ECS::EntityClass[]>(entityCount_),
			std::make_unique_for_overwrite<EntityComponentMemory>()
		};
		}
	);

	for (uint32_t i = 0; i < count; i++) {
		// アクセスするグループのID
		const uint32_t groupId = i + beforeCount;
		// グループの差分
		const uint32_t offset = groupId * entityCount_;
		// グループ内のIndex
		for (uint32_t index = 0; index < entityCount_; index++) {
			const uint32_t totalIndex = offset + index;

			auto &entity = entityStorage_[groupId].first[index];
			entity.chunk_ = chunk_;
			entity.totalIndex_ = totalIndex;
			entity.version_ = 0u;

		}

	}

	// データへのアクセッサ
	return { &*newBegin, count };

}

void ECS::EntityArrayStorage::erase(const std::vector<bool> &flagArray, const size_t trueCount, const uint32_t count)
{
	// もし死んだ要素数がゼロなら終わり
	if (not trueCount) { return; }

	// 生きている要素数
	const uint32_t aliveCount = count - static_cast<uint32_t>(trueCount);

	// 先頭から､書き込み可能の場所を探す
	for (uint32_t i = 0; i < aliveCount; i++) {
		// もし､フラグが折れていたら次へ
		if (not flagArray[i]) { continue; }
		// 逆に､書き込み可能なら

		// 上書きしたというフラグを立てる
		GetEntity(i).version_++;
	}
}

std::byte &ECS::GetComp(Chunk *chunk, uint32_t compId, uint32_t index)
{
	return chunk->componentDatas_.at(compId).at(index);
}

ECS::ComponentData &ECS::GetCompArray(ECS::Chunk *chunk, uint32_t compId)
{
	return chunk->componentDatas_.at(compId);
}

ECS::EntityClass &ECS::GetEntity(Chunk *chunk, uint32_t index)
{
	return chunk->GetEntity(index);
}
void ECS::EntityMove(Chunk *chunk, uint32_t dst, uint32_t src)
{
	// コンポーネントの配列にアクセスして､データを書き換えてコピーする
	for (auto &[key, comp] : chunk->componentDatas_) {
		// 自分自身のメモリ
		std::byte *dstMem = &comp.at(dst);
		// 相手のメモリ
		std::byte *srcMem = &comp.at(src);

		// メモリを書き換えて渡す
		std::memcpy(dstMem, srcMem, comp.GetTypeSize());
	}

	// バージョンを加算する
	chunk->GetEntity(dst).version_++;

}
