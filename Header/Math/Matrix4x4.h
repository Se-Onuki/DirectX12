#pragma once
#include "Vector4.h"

struct Vector3;

struct Matrix4x4 final {
	enum EulerAngle {
		Pitch, // x軸
		Yaw,   // y軸
		Roll   // z軸
	};

	inline Matrix4x4()
		: m{
			  {0, 0, 0, 0},
			  {0, 0, 0, 0},
			  {0, 0, 0, 0},
			  {0, 0, 0, 0}
		} {}

	inline Matrix4x4(
		float A, float B, float C, float D, float E, float F, float G, float H, float I, float J,
		float K, float L, float M, float N, float O, float P)
		: m{
			  {A, B, C, D},
			  {E, F, G, H},
			  {I, J, K, L},
			  {M, N, O, P}
		} {}

	inline Matrix4x4(const Vector4 &A, const Vector4 &B, const Vector4 &C, const Vector4 &D) {
		std::memcpy(m[0], &A, sizeof(Vector4));
		std::memcpy(m[1], &B, sizeof(Vector4));
		std::memcpy(m[2], &C, sizeof(Vector4));
		std::memcpy(m[3], &D, sizeof(Vector4));
	}

	float m[4][4];

	void Printf(const int &x, const int &y) const;

	/// @brief 逆行列関数
	/// @return 逆行列
	Matrix4x4 Inverse() const;

	/// @brief スケーリング無効の逆行列
	/// @return 逆行列
	Matrix4x4 InverseRT() const;

	/// @brief 転置行列関数
	/// @return 転置行列
	Matrix4x4 Transpose() const;

	Matrix4x4 operator+(const Matrix4x4 &Second) const;

	Matrix4x4 operator-(const Matrix4x4 &Second) const;

	Matrix4x4 operator*(const Matrix4x4 &Second) const;

	Matrix4x4 operator*(const float &Second) const;
	Matrix4x4 operator/(const float &Second) const;

	Matrix4x4 operator+=(const Matrix4x4 &Second);

	Matrix4x4 operator-=(const Matrix4x4 &Second);

	Matrix4x4 operator*=(const Matrix4x4 &Second);

	Matrix4x4 operator*=(const float &Second);
	Matrix4x4 operator/=(const float &Second);

	bool operator==(const Matrix4x4 &Second) const {
		return *this == Second;
	}

	static Matrix4x4 Affine(const Vector3 &scale, const Vector3 &rotate, const Vector3 &translate);

	static Matrix4x4 EulerRotate(EulerAngle, float angle);
	static Matrix4x4 EulerRotate(const Vector3 &angle);

	/// @brief 単位行列関数
	/// @return 単位行列
	static Matrix4x4 Identity() {
		return Matrix4x4{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	}
};