/// @file ComponentData.cpp
/// @brief コンポーネントのSpanのように振る舞う
/// @author ONUKI seiya
#include "ComponentData.h"
#include "EntityArrayStorage.h"

namespace ECS {

	std::span<std::byte> ComponentSpan::GetCompArray(uint32_t index)
	{
		return { entityStorage_->GetEntityStorage()[index].second->data() + offset_, itemCount_ * typeSize_ };
	}

	std::byte *ComponentSpan::operator[](uint32_t index)
	{
		return (entityStorage_->GetEntityStorage()[index / itemCount_].second->data() + offset_) + (index % itemCount_) * typeSize_;
	}

	const std::byte *ComponentSpan::operator[](uint32_t index) const
	{
		return entityStorage_->GetEntityStorage()[index / itemCount_].second->data() + offset_ + (index % itemCount_) * typeSize_;
	}

	void ComponentSpan::erase(const std::vector<bool> &flagArray, const size_t delCount, const uint32_t count)
	{
		// もし死んでいる要素数がゼロなら終わり
		if (not delCount) { return; }
		// 全部死んでいたら終わり
		if (delCount == count) { return; }
		// アクセス先のフラグ
		uint32_t front = count - static_cast<uint32_t>(delCount) - 1;	// 前側
		uint32_t back = front + 1;	// 後ろ側

		// 先頭から､書き込み可能の場所を探す
		while (true) {
			// もし､フラグが折れていたら次へ
			if (not flagArray[front]) {
				// 0まで行ったら終わり
				if (front-- == 0) { break; }
				continue;
			}

			// 範囲外でデータが生きている箇所を探す
			for (; back < count; back++) {
				// 折れていたら飛ばす
				if (flagArray[back]) { continue; }

				// 型をコピーする
				std::memcpy(&at(front), &at(back), typeSize_);

				// 次に移動する
				back++;
				// 次のコピー先を探しに行く
				break;
			}

			// 0まで行ったら終わり
			if (front-- == 0) { break; }
		}
	}
}