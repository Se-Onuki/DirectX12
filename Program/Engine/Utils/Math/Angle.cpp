/// @file Angle.cpp
/// @brief 角度に関するクラス
/// @author ONUKI seiya
#include "Angle.h"
#include <cmath>
namespace SoLib {

	Vector2 Angle::RadToVec2(float radian) {
		return Vector2{ std::cosf(radian),std::sinf(radian) };
	}

	float Angle::Vec2ToRad(const Vector2 vec) {
		return std::atan2(vec.y, vec.x);
	}

	float Angle::Lerp(const float start, const float end, const float t) {
		const float diff = end - start;

		return start + Mod(diff) * t;
	}

	Vector3 Angle::Lerp(const Vector3 &start, const Vector3 &end, const float t) {
		const Vector3 &diff = end - start;

		return Mod(start + Mod(diff) * t);
	}

	float Angle::Mod(float radian) {
		// 360度に補完する
		radian = std::fmodf(radian, PI2);

		// 180度に補完する
		if (radian > PI) radian -= PI2;	// +180以上は-360する
		else if (radian < -PI) radian += PI2; // -180以下は+360する

		return radian;
	}

	Vector3 Angle::Mod(const Vector3 &euler) {
		Vector3 result{};
		for (uint8_t i = 0u; i < 3u; i++) {
			(&result.x)[i] = Angle::Mod((&euler.x)[i]);
		}
		return result;
	}

	Angle::Digree::Digree(const Radian &value) : angle_(value.Get() *Angle::Rad2Dig) {  }

	//inline Angle::Digree &Angle::Digree::operator=(const Radian &radian) {
	//	*this = static_cast<Digree>(radian);
	//	return *this;
	//}

	inline Angle::Digree::operator Angle::Radian() const noexcept { return Radian{ angle_ * Dig2Rad }; }

}