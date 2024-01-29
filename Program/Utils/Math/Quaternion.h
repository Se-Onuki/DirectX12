#pragma once
#include <immintrin.h>
#include "SimdCalc.h"
#include "Vector3.h"
#include <array>
#include <cmath>
#include "Matrix4x4.h"
#include "../SoLib/SoLib_Lerp.h"
//#include "Euler.h"
namespace SoLib::Math {

	struct Euler;
}

struct Quaternion final {
	Quaternion() = default;
	Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {};
	Quaternion(const Vector3 &vec, float w = 0.f) :w(w) { this->vec() = vec; };

	float x, y, z;
	float w;

	/// @brief ベクトル部を取得する関数
	/// @return クォータニオンのベクトル部 [ 参照 ]
	inline Vector3 &vec() noexcept { return *reinterpret_cast<Vector3 *>(&x); }
	inline const Vector3 &vec() const noexcept { return *reinterpret_cast<const Vector3 *>(&x); }

	/// @brief 単位クォータニオン
	static const Quaternion Identity;

	/// @brief 共役クォータニオン関数
	/// @return 共役クォータニオン
	inline Quaternion Conjugation() const;

	/// @brief 逆クォータニオン関数
	/// @return 逆クォータニオン
	inline Quaternion Inverse() const;

	/// @brief 二乗ノルム
	/// @return 二乗したクォータニオンの長さ
	inline float LengthSQ() const;

	/// @brief ノルム
	/// @return クォータニオンの長さ
	inline float Length() const;

	/// @brief 正規化関数
	/// @return 単位クォータニオン
	inline Quaternion Normalize() const;

	Matrix4x4 MakeRotateMatrix() const;

	static inline Vector3 RotateVector(const Vector3 &a, const Quaternion &b);
	inline Vector3 RotateVector(const Vector3 &v) const;

	static Quaternion AnyAxisRotation(const Vector3 &axis, float angle);

	static Quaternion Create(const SoLib::Math::Euler &euler);

	static Quaternion Slerp(const Quaternion &start, const Quaternion &end, float t);

	static Quaternion LookAt(const Vector3 &direction);

	/// @brief 明示的な型変換
	inline explicit operator __m128() const noexcept { return _mm_load_ps(&x); }
	inline Quaternion &operator=(const __m128 &vec) noexcept { _mm_store_ps(&x, vec); return *this; }

};

inline Quaternion operator*(const Quaternion &a, const Quaternion &b) {
	Quaternion result;

	// aとbのベクトル部を取得 ( 実数部は 双方切り捨てられる )
	std::array<SoLib::Math::SIMD128, 2u> vec{ static_cast<__m128>(a),static_cast<__m128>(b) };

	// ベクトル部の算出 ( 実数部は Vector3への代入時に切り捨て )
	result.vec() = SoLib::Math::SIMD128::Cross(vec[0u], vec[1u]) + vec[0u] * b.w + vec[1u] * a.w;
	// 実数部の算出 ( ドット積で 実数部を切り捨て )
	result.w = a.w * b.w - SoLib::Math::SIMD128::Dot<3u>(vec[0u], vec[1u]);

	return result;
}

inline Quaternion &operator*=(Quaternion &a, const Quaternion &b) {

	// aとbのベクトル部を取得 ( 実数部は 双方切り捨てられる )
	std::array<SoLib::Math::SIMD128, 2u> vec{ static_cast<__m128>(a),static_cast<__m128>(b) };

	// ベクトル部の算出 ( 実数部は Vector3への代入時に切り捨て )
	a.vec() = SoLib::Math::SIMD128::Cross(vec[0u], vec[1u]) + vec[0u] * b.w + vec[1u] * a.w;
	// 実数部の算出 ( ドット積で 実数部を切り捨て )
	a.w = a.w * b.w - SoLib::Math::SIMD128::Dot<3u>(vec[0u], vec[1u]);

	return a;
}

inline Quaternion Quaternion::Conjugation() const {
	Quaternion result;

	// 符号反転
	result = SoLib::Math::SIMD128{ static_cast<__m128>(*this) } *-1.f;
	// 実数部を戻す
	result.w = this->w;

	return result;
}

inline Quaternion Quaternion::Inverse() const {
	const float lengthSQ = this->LengthSQ();
	if (lengthSQ == 0.f) { return *this; }

	Quaternion result;

	result = SoLib::Math::SIMD128{ static_cast<__m128>(this->Conjugation()) } / lengthSQ;

	return result;
}

inline Quaternion Quaternion::Normalize() const {
	// 長さ
	float length = this->Length();
	// もし長さが0ならそのまま返す( 0なので )
	if (length == 0.f) { return *this; }

	Quaternion result;
	// ノルムで割る
	result = SoLib::Math::SIMD128{ static_cast<__m128>(*this) } / length;
	return result;
}

inline Matrix4x4 Quaternion::MakeRotateMatrix() const {
	return Matrix4x4{
		Vector4{w * w + x * x - y * y - z * z, 2.f * (x * y + w * z), 2.f * (x * z - w * y),0.f},
		Vector4{2.f * (x * y - w * z), w * w - x * x + y * y - z * z,2.f * (y * z + w * x),0.f },
		Vector4{2.f * (x * z + w * y),2.f * (y * z - w * x),w * w - x * x - y * y + z * z,0.f},
		Vector4{0.f, 0.f, 0.f, 1.f}
	};
}

inline Quaternion Quaternion::AnyAxisRotation(const Vector3 &axis, float angle) {
	float halfAngle = angle / 2.f;
	return Quaternion{ axis * std::sin(halfAngle), std::cos(halfAngle) };
}


inline float Quaternion::LengthSQ() const {
	__m128 value = static_cast<__m128>(*this);
	return SoLib::Math::SIMD128::Dot<4u>(value, value);
}

inline float Quaternion::Length() const {
	__m128 value = static_cast<__m128>(*this);
	return std::sqrt(SoLib::Math::SIMD128::Dot<4u>(value, value));
}

inline Vector3 Quaternion::RotateVector(const Vector3 &a, const Quaternion &b) {

	return b.RotateVector(a);
}

inline Vector3 Quaternion::RotateVector(const Vector3 &v) const {
	Quaternion result = *this * Quaternion{ v } *this->Conjugation();

	return result.vec();
}

inline Quaternion Quaternion::Slerp(const Quaternion &start, const Quaternion &end, float t) {

	std::array<SoLib::Math::SIMD128, 2u> vec{ static_cast<__m128>(start),static_cast<__m128>(end) };

	Quaternion result;
	float dot = SoLib::Math::SIMD128::Dot<4u>(vec[0], vec[1]);

	if (dot < 0) {
		vec[0] = -vec[0];
		dot = -dot;
	}
	if (dot >= 1.f - std::numeric_limits<float>::epsilon()) {
		result = SoLib::Lerp(vec[0], vec[1], t);
		return result;
	}

	const float theta = std::acos(dot);

	const float sin = std::sin(theta);

	result = vec[0] * (std::sin((1 - t) * theta) / sin) + vec[1] * std::sin(t * theta) / sin;
	return result;
}

inline Quaternion Quaternion::LookAt(const Vector3 &direction) {
	float dot = Vector3::front * direction;
	dot = std::clamp(dot, -1.f, 1.f);
	const float theta = std::acos(dot); //角度の算出
	const Vector3 cross = Vector3::front.cross(direction).Nomalize(); //中心ベクトル

	return Quaternion::AnyAxisRotation(cross, theta).Normalize();

}
