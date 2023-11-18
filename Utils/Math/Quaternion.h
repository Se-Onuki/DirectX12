#pragma once
#include <immintrin.h>
#include "SimdCalc.h"
#include "Vector3.h"
#include <array>
#include <cmath>

struct Quaternion final {
	float x, y, z;
	float w;

	/// @brief ベクトル部を取得する関数
	/// @return クォータニオンのベクトル部 [ 参照 ]
	inline Vector3 &vec() noexcept { return *reinterpret_cast<Vector3 *>(&x); }
	inline const Vector3 &vec() const noexcept { return *reinterpret_cast<const Vector3 *>(&x); }

	/// @brief 単位クォータニオン
	static const Quaternion Identity;

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

	/// @brief 明示的な型変換
	inline explicit operator __m128() const noexcept { return _mm_load_ps(&x); }
	inline Quaternion &operator=(const __m128 &vec) noexcept { _mm_store_ps(&x, vec); return *this; }

};

inline Quaternion operator*(const Quaternion &a, const Quaternion &b) {
	Quaternion result;

	// aとbのベクトル部を取得
	std::array<SoLib::Math::SIMD128, 2u> vec{ static_cast<__m128>(a),static_cast<__m128>(b) };

	// ベクトル部の算出
	result.vec() = SoLib::Math::SIMD128{ SoLib::Math::SIMD128::Cross(vec[0u], vec[1u]) } + vec[0u] * b.w + vec[1u] * a.w;
	// 実数部の算出
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

inline float Quaternion::LengthSQ() const {
	__m128 value = static_cast<__m128>(*this);
	return SoLib::Math::SIMD128::Dot<4u>(value, value);
}

inline float Quaternion::Length() const {
	__m128 value = static_cast<__m128>(*this);
	return std::sqrt(SoLib::Math::SIMD128::Dot<4u>(value, value));
}
