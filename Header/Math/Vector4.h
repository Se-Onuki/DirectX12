#pragma once

#include <cmath>

struct Vector4 {
	// Vector4(float x = 0.f, float y = 0.f, float z = 0.f, float w = 0.f) : x(x), y(y), z(z), w(w) {}

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
	float Length() const { return sqrtf((*this) * (*this)); }

	/// <summary>
	/// 正規化
	/// </summary>
	/// <returns>ベクトル長が1のベクトル</returns>
	Vector4 Nomalize() const {

		float Length = this->Length();
		if (Length != 0) {
			return *this / Length;
		} else {
			return zero();
		}
	}

	Vector4 operator+(const Vector4& Second) const {
		return Vector4{
		    this->x + Second.x, this->y + Second.y, this->z + Second.z, this->w + Second.w};
	}
	Vector4 operator-(const Vector4& Second) const {
		return Vector4{
		    this->x - Second.x, this->y - Second.y, this->z - Second.z, this->w - Second.w};
	}

	Vector4& operator+=(const Vector4& Second) {
		this->x += Second.x;
		this->y += Second.y;
		this->z += Second.z;
		this->w += Second.w;
		return *this;
	}
	Vector4& operator-=(const Vector4& Second) {
		this->x -= Second.x;
		this->y -= Second.y;
		this->z -= Second.z;
		this->w -= Second.w;
		return *this;
	}

	Vector4 operator*(const float& Second) const {
		return Vector4{this->x * Second, this->y * Second, this->z * Second, this->w * Second};
	}
	Vector4 operator/(const float& Second) const {
		return Vector4{this->x / Second, this->y / Second, this->z / Second, this->w / Second};
	}

	Vector4& operator*=(const float& Second) {
		this->x *= Second;
		this->y *= Second;
		this->z *= Second;
		this->w *= Second;
		return *this;
	}
	Vector4& operator/=(const float& Second) {
		this->x /= Second;
		this->y /= Second;
		this->z /= Second;
		this->w /= Second;
		return *this;
	}

	// Vector3 operator*(const Matrix3x3& Second) const {}

	// Vector3 operator*=(const Matrix3x3& Second) {}

	// Vector4 operator*(const Matrix4x4& Second) const;

	// Vector4& operator*=(const Matrix4x4& Second);

	// 逆ベクトル
	inline Vector4 operator-() const { return *this * -1; }

	// 内積
	inline float operator*(const Vector4& v) const { return x * v.x + y * v.y + z * v.z + w * v.w; }
	// 外積
	// inline float operator^(const Vector3& v) const { return x * v.y - y * v.x; }

	inline static Vector4 zero() { return Vector4{0, 0, 0, 0}; }

	// inline Vector4 Reflect(Vector4 normal) const {
	//	return (*this) - normal * 2 * ((*this) * normal);

	//	// return {this->x- 2}
	//}

	inline bool operator==(const Vector4& vec) const {
		return (this->x == vec.x) && (this->y == vec.y) && (this->z == vec.z) && (this->w == vec.w);
	}

private:
};