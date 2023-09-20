#pragma once
#include <stdint.h>

/// 参考サイト
/// https://easings.net/ja
///

namespace SoLib {
	inline float easeLinear(float number) { return number; }

	float easeInSine(float x);
	float easeOutSine(float x);
	float easeInOutSine(float x);

	float easeInQuad(float x);
	float easeOutQuad(float x);
	float easeInOutQuad(float x);

	float easeInBack(float x);
	float easeOutBack(float x);
	float easeInOutBack(float x);

	float easeInCirc(float x);
	float easeOutCirc(float x);

	float easeInExpo(float x);
	float easeOutExpo(float x);

	float easeInOutQuint(float x);


	float easeOutBounce(float x);

	float easeOutElastic(float x);


	/// @brief 色用の線形補間
	/// @param statColor 始点RGBA色
	/// @param endColor 終点RGBA色
	/// @param easingVolume 係数
	/// @return 計算結果
	uint32_t ColorLerp(uint32_t statColor, uint32_t endColor, float easingVolume);
}