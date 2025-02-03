/// @file FloatArray.h
/// @brief floatが2個の配列
/// @author ONUKI seiya
#pragma once
#include <array>
#include <stdint.h>
namespace SoLib {

	struct Float2 {
		std::array<float, 2u> value;
	};
}