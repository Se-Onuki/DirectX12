#pragma once

#include <cmath>
#include <immintrin.h>
#include "SimdCalc.h"

struct Vector4 {

	float x;
	float y;
	float z;
	float w;

	/// <summary>
	/// ベクトルをデバック表示する
	/// </summary>
	/// <param name="x">x座標</param>
	/// <param name="y">y座標</param>
	// void Printf(int x, int y) const;

	/// <summary>
	/// ベクトル長関数
	/// </summary>
	/// <returns>ベクトルの長さ</returns>
	float Length() const {
		return std::sqrt((*this) * (*this));
	}

	/// <summary>
	/// ベクトル長関数
	/// </summary>
	/// <returns>ベクトルの長さ</returns>
	float LengthSQ() const {
		return (*this) * (*this);
	}

	/// <summary>
	/// 正規化
	/// </summary>
	/// <returns>ベクトル長が1のベクトル</returns>
	Vector4 Nomalize() const {

		float Length = this->Length();
		if (Length != 0) {
			return *this / Length;
		}
		else {
			return zero;
		}
	}

	Vector4 operator+(const Vector4 &v) const {
		Vector4 result;
		for (uint32_t i = 0u; i < 4u; i++) {
			(&result.x)[i] = (&this->x)[i] + (&v.x)[i];
		}
		return result;
	}

	Vector4 operator-(const Vector4 &v) const {
		Vector4 result;
		for (uint32_t i = 0u; i < 4u; i++) {
			(&result.x)[i] = (&this->x)[i] - (&v.x)[i];
		}
		return result;
	}

	Vector4 &operator+=(const Vector4 &v) {

		for (uint32_t i = 0u; i < 4u; i++) {
			(&this->x)[i] += (&v.x)[i];
		}
		return *this;
	}
	Vector4 &operator-=(const Vector4 &v) {
		for (uint32_t i = 0u; i < 4u; i++) {
			(&this->x)[i] -= (&v.x)[i];
		}
		return *this;
	}

	Vector4 operator*(const float &value) const {
		Vector4 result;
		for (uint32_t i = 0u; i < 4u; i++) {
			(&result.x)[i] = (&this->x)[i] * value;
		}
		return result;
	}
	Vector4 operator/(const float &value) const {
		Vector4 result;
		for (uint32_t i = 0u; i < 4u; i++) {
			(&result.x)[i] = (&this->x)[i] / value;
		}
		return result;
	}

	Vector4 &operator*=(const float &value) {
		return *this = *this * value;
	}

	Vector4 &operator/=(const float &value) {
		return *this = *this / value;
	}

	// Vector3 operator*(const Matrix3x3& Second) const {}

	// Vector3 operator*=(const Matrix3x3& Second) {}

	// Vector4 operator*(const Matrix4x4& Second) const;

	// Vector4& operator*=(const Matrix4x4& Second);

	// 逆ベクトル
	inline Vector4 operator-() const {
		return *this * -1;
	}

	// 内積
	inline float operator*(const Vector4 &v) const {
		float result{};
		for (uint32_t i = 0u; i < 4u; i++) {
			result += (&this->x)[i] + (&v.x)[i];
		}
		return result;
	}
	// 外積
	// inline float operator^(const Vector3& v) const { return x * v.y - y * v.x; }

	static const Vector4 zero;
	static const Vector4 one;

	// inline Vector4 Reflect(Vector4 normal) const {
	//	return (*this) - normal * 2 * ((*this) * normal);

	//	// return {this->x- 2}
	//}

	inline bool operator==(const Vector4 &vec) const {
		return (this->x == vec.x) && (this->y == vec.y) && (this->z == vec.z) && (this->w == vec.w);
	}

	static uint32_t size() { return 4u; }

	float *const begin() { return &x; }
	const float *const begin() const { return &x; }
	const float *const cbegin() const { return &x; }

	float *const end() { return begin() + size(); }
	const float *const end() const { return begin() + size(); }
	const float *const cend() const { return begin() + size(); }

	float *const data() { return begin(); }
	const float *const data() const { return begin(); }
	const float *const cdata() const { return begin(); }


	inline explicit operator __m128() const { return _mm_load_ps(&x); }
	inline Vector4 &operator=(const __m128 &vec) { _mm_store_ps(reinterpret_cast<float *>(this), vec); return *this; }

	inline explicit operator SoLib::Math::SIMD128() const { return SoLib::Math::SIMD128{ static_cast<__m128>(*this) }; }


private:
};
