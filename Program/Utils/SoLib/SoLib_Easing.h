#pragma once
#include <stdint.h>
#include "SoLib_ImGui.h"

#define StringAndValue(x)  #x, x 

/// 参考サイト
/// https://easings.net/ja
///

namespace SoLib {

	inline constexpr float easeLinear(float number) { return number; }

	constexpr float easeInSine(float x);
	constexpr float easeOutSine(float x);
	constexpr float easeInOutSine(float x);

	constexpr float easeInQuad(float x);
	constexpr float easeOutQuad(float x);
	constexpr float easeInOutQuad(float x);

	constexpr float easeInBack(float x);
	constexpr float easeOutBack(float x);
	constexpr float easeInOutBack(float x);

	constexpr float easeInCirc(float x);
	constexpr float easeOutCirc(float x);

	constexpr float easeInExpo(float x);
	constexpr float easeOutExpo(float x);

	constexpr float easeInOutQuint(float x);


	constexpr float easeOutBounce(float x);

	constexpr float easeOutElastic(float x);

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
	bool ImGuiWidget<EaseFunc>(const char *const label, EaseFunc *const value) {

		return SoLib::ImGuiWidget(label, &kFuncList, value->easeFunc_, [](const uint32_t item)->std::string {return kFuncList.at(item).first; });

	}

}