#include "SoLib_Easing.h"
#include <cmath>
#include <algorithm>

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
			: x ==1.0f
			? 1.0f
			: std::pow(2.0f, -10.0f * x) * std::sin((x * 10.0f - 0.75f) * c4) + 1.0f;
	}



	uint32_t ColorLerp(uint32_t statColor, uint32_t endColor, float easingVolume) {

		uint32_t redColor = std::clamp<uint32_t>(static_cast<uint32_t>(std::lerp<uint32_t>((((statColor & 0xFF000000) >> (4 * 6)) & 0xFF), (((endColor & 0xFF000000) >> (4 * 6)) & 0xFF), easingVolume)), 0x00, 0xFF);
		uint32_t greenColor = std::clamp<uint32_t>(static_cast<uint32_t>(std::lerp<uint32_t>((((statColor & 0x00FF0000) >> (4 * 4)) & 0xFF), (((endColor & 0x00FF0000) >> (4 * 4)) & 0xFF), easingVolume)), 0x00, 0xFF);
		uint32_t blueColor = std::clamp<uint32_t>(static_cast<uint32_t>(std::lerp<uint32_t>((((statColor & 0x0000FF00) >> (4 * 2)) & 0xFF), (((endColor & 0x0000FF00) >> (4 * 2)) & 0xFF), easingVolume)), 0x00, 0xFF);

		uint32_t alphaColor = std::clamp<uint32_t>(static_cast<uint32_t>(std::lerp<uint32_t>((statColor & 0x000000FF), (endColor & 0x000000FF), easingVolume)), 0x00, 0xFF);

		return (redColor << (4 * 6)) + (greenColor << (4 * 4)) + (blueColor << (4 * 2)) + (alphaColor);

	}

}