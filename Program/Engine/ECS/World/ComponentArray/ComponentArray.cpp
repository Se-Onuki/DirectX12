/// @file ComponentArray.cpp
/// @brief コンポーネントの配列
/// @author ONUKI seiya
#include "ComponentArray.h"

namespace ECS {

	void ComponentArray::MoveElement(const std::vector<bool>::iterator flags, const uint32_t trueCount, const uint32_t elemCount)
	{
		// もし要素数がゼロなら終わり
		if (not elemCount) { return; }
		// アクセス先のフラグ
		uint32_t front = 0;				// 開始地点
		uint32_t back = elemCount - 1;	// 終了地点

		// 先頭から､書き込み可能の場所を探す
		for (; front < trueCount; front++) {
			// もし､フラグが立っていたら終わり
			if (flags[front]) { continue; }
			// 逆に､書き込み可能なら

			// 範囲外でデータが生きている箇所を探す
			for (; back >= trueCount; back--) {
				// 死んでいたら飛ばす
				if (not flags[back]) { continue; }
				// 生きていたら

				// 型をコピーする
				std::memcpy(at(front), at(back), typeSize_);

				// 次のコピー先を探しに行く
				break;
			}
		}
	}
}