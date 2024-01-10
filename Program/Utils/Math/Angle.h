#pragma once
#include <numbers>
#include "Vector2.h"
#include "Vector3.h"

namespace Angle {

	constexpr float PI = static_cast<float>(std::numbers::pi);
	constexpr float hPI = PI * 0.5f;
	constexpr float PI2 = PI * 2.f;

	constexpr float Dig2Rad = PI / 180.f;
	constexpr float Rad2Dig = 180.f / PI;

	class Radian;
	class Digree {
	public:
		Digree() = default;
		Digree(float value) :angle_(value) {};
		Digree(const Radian &value);
		Digree(const Digree &) = default;
		inline Digree &operator=(const Digree &digree) {
			angle_ = digree.angle_;
			return *this;
		}

		~Digree() = default;

		inline Digree operator+() const { return angle_; }
		inline Digree operator-() const { return -angle_; }

		/// @brief 非明示的にDigreeを[ Radian ]に変換できる
		inline operator Angle::Radian() const noexcept;

		const float &Get() const noexcept { return angle_; }
		float &Get() noexcept { return angle_; }

	private:
		// 角度の値
		float angle_;
	};

	class Radian {
	public:
		Radian() = default;
		Radian(float value) :angle_(value) {};
		Radian(const Digree &value) { *this = static_cast<Radian>(value); };
		Radian(const Radian &) = default;
		inline Radian &operator=(const Radian &radian) {
			angle_ = radian.angle_;
			return *this;
		}

		~Radian() = default;

		inline Radian operator+() const { return angle_; }
		inline Radian operator-() const { return -angle_; }
		/// @brief 非明示的にRadianを[ float ]に変換できる
		//inline operator const float &() const noexcept { return angle_; }

		/// @brief 明示的にRadianを[ Digree ]に変換できる
		inline explicit operator Digree () const noexcept { return Digree{ angle_ * Rad2Dig }; }

		inline Radian &operator=(const Digree &value) noexcept { return *this = static_cast<Radian>(value); }

		const float &Get() const noexcept { return angle_; }
		float &Get() noexcept { return angle_; }

	private:
		// 角度の値
		float angle_;
	};


	Vector2 RadToVec2(float radian);
	float Vec2ToRad(const Vector2 &vec);

	float Lerp(const float start, const float end, const float t);
	Vector3 Lerp(const Vector3 &start, const Vector3 &end, const float t);

	float Mod(float radian);
	Vector3 Mod(const Vector3 &euler);


} // namespace Angle

/// @brief 度数法の値を弧度法に変換
/// @param val 度数法
/// @return 弧度法
inline float operator""_deg(long double val) {
	return static_cast<float>(val) * Angle::Dig2Rad;
}