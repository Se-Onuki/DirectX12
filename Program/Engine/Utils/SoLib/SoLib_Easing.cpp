#include "SoLib_Easing.h"
#include <algorithm>
#include <cmath>

#include "../Math/Angle.h"

namespace SoLib {

	float easeInSine(float x) {
		return 1.f - std::cos((x * Angle::PI) / 2.f);
	}

	float easeOutSine(float x) {
		return std::sin((x * Angle::PI) / 2.f);
	}

	float easeInOutSine(float x) {
		return -(std::cos(Angle::PI * x) - 1.f) / 2.f;
	}

	float easeInQuad(float x) {
		return x * x;
	}

	float easeOutQuad(float x) {
		return 1.f - (1.f - x) * (1.f - x);
	}

	float easeInOutQuad(float x) {
		return x < 0.5f ? 2.f * x * x : 1.f - std::pow(-2.f * x + 2.f, 2.f) / 2.f;
	}

	float easeInBack(float x) {
		const float c1 = 1.70158f;
		const float c3 = c1 + 1.f;

		return c3 * std::pow(x, 3.f) - c1 * std::pow(x, 2.f);
	}

	float easeOutBack(float x) {
		const float c1 = 1.70158f;
		const float c3 = c1 + 1.f;

		return 1.f + c3 * std::pow((x)-1.f, 3.f) + c1 * std::pow((x)-1.f, 2.f);
	}

	float easeInOutBack(float x) {
		const float c1 = 1.70158f;
		const float c2 = c1 * 1.525f;

		return x < 0.5f
			? (std::pow(2.0f * x, 2.0f) * ((c2 + 1.0f) * 2.0f * x - c2)) / 2.0f
			: (std::pow(2.0f * x - 2.0f, 2.0f) * ((c2 + 1.0f) * (x * 2.0f - 2.0f) + c2) + 2.0f) / 2.0f;
	}

	float easeInCirc(float x) {
		return 1.f - std::sqrt(1.f - std::pow(x, 2.f));
	}

	float easeOutCirc(float x) {
		return std::sqrt(1.f - std::pow(x - 1.f, 2.f));
	}

	float easeInExpo(float x) {
		return x = (x == 0.f) ? 0.f : std::pow(2.f, 10.f * x - 10.f);
	}

	float easeOutExpo(float x) {
		return x = (x == 1.f) ? 1.f : 1.f - std::pow(2.f, -10.f * x);
	}

	float easeInOutQuint(float x) {
		return x < 0.5f ? 16.0f * x * x * x * x * x : 1 - std::pow(-2.0f * x + 2.0f, 5.0f) / 2.0f;
	}

	float easeOutBounce(float x) {
		const float n1 = 7.5625f;
		const float d1 = 2.75f;

		if (x < 1.0f / d1) {
			return n1 * x * x;
		}
		else if (x < 2.0f / d1) {
			return n1 * (x -= 1.5f / d1) * x + 0.75f;
		}
		else if (x < 2.5f / d1) {
			return n1 * (x -= 2.25f / d1) * x + 0.9375f;
		}
		else {
			return n1 * (x -= 2.625f / d1) * x + 0.984375f;
		}
	}

	float easeOutElastic(float x) {
		const float c4 = (2 * std::numbers::pi_v<float>) / 3.0f;

		return x == 0.0f
			? 0.0f
			: x == 1.0f
			? 1.0f
			: std::pow(2.0f, -10.0f * x) * std::sin((x * 10.0f - 0.75f) * c4) + 1.0f;
	}


	template<>
	bool ImGuiWidget(const char *const label, EaseFunc *const value) {
		return SoLib::ImGuiWidget(label, &kFuncList, value->easeFunc_, [](const uint32_t item)->std::string {return kFuncList.at(item).first; });
	}

}