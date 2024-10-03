#pragma once
#include <stdint.h>
#include "SoLib_ImGui.h"

#define StringAndValue(x)  #x, x 

/// 参考サイト
/// https://easings.net/ja
///

namespace SoLib {

	inline constexpr float easeLinear(float number) { return number; }

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

	inline static constexpr std::array<std::pair<const char *const, float (*)(float)>, 17> kFuncList{
			{
				{StringAndValue(easeLinear)},

				{StringAndValue(easeInSine)},
				{StringAndValue(easeOutSine)},
				{StringAndValue(easeInOutSine)},

				{StringAndValue(easeInQuad)},
				{StringAndValue(easeOutQuad)},
				{StringAndValue(easeInOutQuad)},

				{StringAndValue(easeInBack)},
				{StringAndValue(easeOutBack)},
				{StringAndValue(easeInOutBack)},

				{StringAndValue(easeInCirc)},
				{StringAndValue(easeOutCirc)},

				{StringAndValue(easeInExpo)},
				{StringAndValue(easeOutExpo)},

				{StringAndValue(easeInOutQuint)},

				{StringAndValue(easeOutBounce)},

				{StringAndValue(easeOutElastic)},
			}
	};

	struct EaseFunc {
		float operator()(float value) {
			return kFuncList[easeFunc_].second(value);
		}

		uint32_t easeFunc_ = 0u;
	};

	template<>
	bool ImGuiWidget<EaseFunc>(const char *const label, EaseFunc *const value);

}