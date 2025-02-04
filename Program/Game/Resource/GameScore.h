/// @file GameScore.h
/// @brief ゲームのスコア
/// @author ONUKI seiya
#pragma once
#include <cstdint>
#include "../Engine/Utils/SoLib/SoLib_Timer.h"

struct GameScore {

	// ゲームスコア
	uint32_t score_;

	size_t killCount_;

	// 生存時間
	SoLib::Time::SecondF aliveTime_;

};