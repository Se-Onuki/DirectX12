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

	struct EaseFunc {
		float operator()(float value) {
			return easeFunc(value);
		}

		float (*easeFunc)(float) = easeLinear;
	};

}