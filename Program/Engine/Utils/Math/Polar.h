/// @file Polar.h
/// @brief 極座標
/// @author ONUKI seiya
#pragma once
#include "Angle.h"
namespace SoLib {

	struct Polar
	{
		// 半径
		float radius_;
		// 角度
		Angle::Radian theta_;

	};
}