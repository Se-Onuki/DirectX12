#include "ComponentData.h"

namespace ECS {
	//void ComponentData::MoveElement(const std::vector<bool> &flagArray, const std::vector<uint32_t> trueCount)
	//{
	//	for (size_t i = 0; i < components_.size(); i++) {
	//		auto &compArray = components_[i];
	//		compArray.MoveElement(flagArray.begin() + i* )
	//	}
	//}

	void ComponentData::erase(const std::vector<bool> &flagArray, const size_t trueCount, const uint32_t count)
	{
		// もし死んでいる要素数がゼロなら終わり
		if (not trueCount) { return; }
		// 全部死んでいたら終わり
		if (trueCount == count) { return; }
		// アクセス先のフラグ
		uint32_t front = count - static_cast<uint32_t>(trueCount) - 1;	// 前側
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

				// 次のコピー先を探しに行く
				break;
			}

			// 0まで行ったら終わり
			if (front-- == 0) { break; }
		}
	}
}