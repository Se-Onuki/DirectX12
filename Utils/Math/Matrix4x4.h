#pragma once
#include "Vector4.h"
#include <immintrin.h>
#include <array>

struct Vector3;

struct Matrix4x4 final {
	enum EulerAngle {
		Pitch, // x軸
		Yaw,   // y軸
		Roll   // z軸
	};

	Matrix4x4() = default;

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

	inline Matrix4x4(const std::array<Vector4, 4u> &vec) {
		std::memcpy(m, vec.data(), sizeof(Matrix4x4));
	}

	float m[4][4];

	void Printf(const int &x, const int &y) const;

	/// @brief 逆行列関数
	/// @return 逆行列
	Matrix4x4 Inverse() const;

	/// @brief スケーリング無効の逆行列
	/// @return 逆行列
	Matrix4x4 InverseRT() const;

	/// @brief Transform逆行列
	/// @return 逆行列
	inline Matrix4x4 InverseSRT() const;

	/// @brief 転置行列関数
	/// @return 転置行列
	Matrix4x4 Transpose() const;

	Matrix4x4 operator+(const Matrix4x4 &Second) const;

	Matrix4x4 operator-(const Matrix4x4 &Second) const;

	inline Matrix4x4 operator*(const Matrix4x4 &Second) const;

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

	/// @brief 指定範囲を切り抜く関数
	/// @tparam row 行(y要素数)
	/// @tparam column 列(x要素数)
	/// @return 切り出した行列(その他は単位行列)
	template <uint8_t row, uint8_t column>
	Matrix4x4 Crop() const;

	Matrix4x4 GetRotate() const { return this->Crop<3u, 3u>(); }

	static Matrix4x4 Affine(const Vector3 &scale, const Vector3 &rotate, const Vector3 &translate);

	static Matrix4x4 EulerRotate(EulerAngle, float angle);
	static Matrix4x4 EulerRotate(const Vector3 &angle);

	static Matrix4x4 AnyAngleRotate(const Vector3 &axis, const float angle);

	static Matrix4x4 AnyAngleRotate(const Vector3 &axis, const float cos, const float sin);

	static Matrix4x4 DirectionToDirection(const Vector3 &from, const Vector3 &to);

	/// @brief 単位行列関数
	/// @return 単位行列
	static Matrix4x4 Identity() {
		return Matrix4x4{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	}

	static Matrix4x4 LookAtLH(
		const Vector3 &cameraPosition, // 視点の位置
		const Vector3 &cameraTarget,   // 目標の位置
		const Vector3 &cameraUpVector  // 上方を示すベクトル
	);

	static uint32_t size() { return 16u; }

	float *const begin() { return *m; }
	const float *const begin() const { return *m; }
	const float *const cbegin() const { return *m; }

	float *const end() { return begin() + size(); }
	const float *const end() const { return end(); }
	const float *const cend() const { return end(); }

	float *const data() { return begin(); }
	const float *const data() const { return begin(); }
	const float *const cdata() const { return begin(); }

};
#pragma region 4x4Func

Matrix4x4 Matrix4x4::operator*(const Matrix4x4 &sec) const {
	Matrix4x4 result;
	__m128 row0 = _mm_load_ps(sec.m[0]);
	__m128 row1 = _mm_load_ps(sec.m[1]);
	__m128 row2 = _mm_load_ps(sec.m[2]);
	__m128 row3 = _mm_load_ps(sec.m[3]);
	for (int i = 0; i < 4; i++) {
		__m128 brod0 = _mm_set1_ps(m[i][0]);
		__m128 brod1 = _mm_set1_ps(m[i][1]);
		__m128 brod2 = _mm_set1_ps(m[i][2]);
		__m128 brod3 = _mm_set1_ps(m[i][3]);
		__m128 row = _mm_add_ps(
			_mm_add_ps(_mm_mul_ps(brod0, row0), _mm_mul_ps(brod1, row1)),
			_mm_add_ps(_mm_mul_ps(brod2, row2), _mm_mul_ps(brod3, row3)));
		_mm_store_ps(result.m[i], row);
	}
	return result;
}

Matrix4x4 Matrix4x4::InverseSRT() const {
	const Vector4 vecX2 =
		*(Vector4 *)&m[0] / _mm_cvtss_f32(_mm_dp_ps(_mm_load_ps(m[0]), _mm_load_ps(m[0]), 0x71));
	const Vector4 vecY2 =
		*(Vector4 *)&m[1] / _mm_cvtss_f32(_mm_dp_ps(_mm_load_ps(m[1]), _mm_load_ps(m[1]), 0x71));
	const Vector4 vecZ2 =
		*(Vector4 *)&m[2] / _mm_cvtss_f32(_mm_dp_ps(_mm_load_ps(m[2]), _mm_load_ps(m[2]), 0x71));

	return Matrix4x4{
		{vecX2.x, vecY2.x, vecZ2.x, 0.f},
		{vecX2.y, vecY2.y, vecZ2.y, 0.f},
		{vecX2.z, vecY2.z, vecZ2.z, 0.f},
		{-_mm_cvtss_f32(_mm_dp_ps(_mm_load_ps(m[3]), *(__m128 *) & vecX2, 0x71)),
		 -_mm_cvtss_f32(_mm_dp_ps(_mm_load_ps(m[3]), *(__m128 *) & vecY2, 0x71)),
		 -_mm_cvtss_f32(_mm_dp_ps(_mm_load_ps(m[3]), *(__m128 *) & vecZ2, 0x71)), 1.f}
	};
};

template<uint8_t row, uint8_t column>
inline Matrix4x4 Matrix4x4::Crop() const {
	Matrix4x4 result = Matrix4x4::Identity();

	for (uint32_t y = 0; y < row; ++y) {
		for (uint32_t x = 0; x < column; ++x) {
			result.m[y][x] = m[y][x];
		}
	}

	return result;
}
//
///// @brief 文字列変換
///// @param matrix 
///// @return 
//std::string SoLib::to_string(const Matrix4x4 &matrix) {
//	std::string result = "[\n";
//	for (uint32_t y = 0u; y < 4u; ++y) {
//		result += "  [ ";
//		for (uint32_t x = 0u; x < 4u; ++x) {
//			result += std::to_string(matrix.m[y][x]) + ",\t";
//			if (x < 3u) {
//				result += ", ";
//			}
//		}
//		result += " ],\n";
//	}
//	result += "]";
//	return result;
//}

#pragma endregion
