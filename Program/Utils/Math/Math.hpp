#pragma once

#include <numbers>

#include "Angle.h"
#include "Euler.h"
#include "Quaternion.h"
#include "Matrix2x3.h"
#include "SimdCalc.h"

struct Polar;

struct Vector2;
struct Vector3;

struct Matrix2x2;
struct Matrix3x3;
struct Matrix4x4;

// static const Vector2 TextSize{ 50, 20 };

/// <summary>
/// 極座標系をベクトルに変換する。
/// </summary>
/// <param name="Polar">極座標</param>
/// <returns>ベクトル</returns>
Vector2 operator<<(Vector2 &vec2, const Polar &Polar);

/// <summary>
/// ベクトルを極座標系に変換する。
/// </summary>
/// <param name="Vector">ベクトル</param>
/// <returns>極座標クラス</returns>
Polar operator<<(Polar &Polar, const Vector2 &vec2);

/// <summary>
/// 回転行列作成関数
/// </summary>
/// <param name="theta">radian角度</param>
/// <returns>2x2の回転行列</returns>
Matrix2x2 MakeRotateMatrix(const float &theta);

// <summary>
/// 拡縮行列作成関数
/// </summary>
/// <param name="scale">拡縮の倍率</param>
/// <returns>3x3の拡縮行列</returns>
Matrix3x3 MakeScaleMatrix(const Vector2 &scale);

// <summary>
/// 拡縮行列作成関数
/// </summary>
/// <param name="scale">拡縮の倍率</param>
/// <returns>4x4の拡縮行列</returns>
Matrix4x4 MakeScaleMatrix(const Vector3 &scale);

// <summary>
/// 回転行列作成関数
/// </summary>
/// <param name="theta">radian角度</param>
/// <returns>3x3の回転行列</returns>
Matrix3x3 MakeRotateMatrix3x3(const float &theta);

/// <summary>
/// 平行移動行列の作成関数
/// </summary>
/// <param name="translate">変換元のベクトル</param>
/// <returns>同次座標系</returns>
Matrix3x3 MakeTranslateMatrix(const Vector2 &translate);

/// <summary>
/// 平行移動行列の作成関数
/// </summary>
/// <param name="translate">変換元のベクトル</param>
/// <returns>同次座標系</returns>
Matrix4x4 MakeTranslateMatrix(const Vector3 &translate);

/// <summary>
/// アフィン行列の作成関数
/// </summary>
/// <param name="scale">拡縮の倍率</param>
/// <param name="theta">radian角度</param>
/// <param name="translate">変換元のベクトル</param>
/// <returns>同次座標系</returns>
Matrix3x3 MakeAffineMatrix(const Vector2 &scale, const float &theta, const Vector2 &translate);
//
///// <summary>
///// ベクトルと同次座標系の合成
///// </summary>
///// <param name="vector">合成前のベクトル</param>
///// <param name="matrix">同次座標系の平行移動行列</param>
///// <returns>合成したベクトル</returns>
// Vector2 Transform(const Vector2 &vector, const Matrix3x3 &matrix);
//
///// <summary>
///// ベクトルと同次座標系の合成
///// </summary>
///// <param name="vector">合成前のベクトル</param>
///// <param name="matrix">同次座標系の平行移動行列</param>
///// <returns>合成したベクトル</returns>
// Vector3 Transform(const Vector3 &vector, const Matrix4x4 &matrix);

/// <summary>
/// 正射影行列
/// </summary>
/// <param name="LeftTop">左上</param>
/// <param name="RightBottom">右下</param>
/// <returns>正射影行列</returns>
Matrix3x3 MakeOrthographicMatrix(const Vector2 &LeftTop, const Vector2 &RightBottom);

/// <summary>
/// ビューポート行列
/// </summary>
/// <param name="LeftTop">左上</param>
/// <param name="RightBottom">右下</param>
/// <returns>ビューポート行列</returns>
Matrix3x3 MakeViewportMatrix(const Vector2 &LeftTop, const Vector2 &RightBottom);

/// @brief ベクトルの向きのみの回転
/// @param v 元ベクトル
/// @param m ワールド行列
/// @return 回転したベクトル
Vector3 TransformNormal(const Vector3 &v, const Matrix4x4 &m);

namespace SoLib {

	namespace Math {

		Matrix4x4 Affine(const Vector3 &scale, const Vector3 &rotate, const Vector3 &translate);
		Matrix4x4 Affine(const Vector3 &scale, const Quaternion &quaternion, const Vector3 &translate);

		Quaternion MakeQuaternion(const SoLib::Math::Euler &euler);

		Vector3 EulerToDirection(const SoLib::Math::Euler &euler);

		SoLib::Math::Euler DirectionToEuler(const Vector3 &direction);

		template <size_t x, size_t y, typename T>
		void ArrayTranspose(const T *input, T *output) {
			for (size_t i = 0; i < x; ++i) {
				for (size_t j = 0; j < y; ++j) {
					output[j * x + i] = input[i * y + j];
				}
			}
		}

		void Add(Matrix2x3 &result, const Matrix2x3 &left, const Matrix2x3 &right) {
			for (uint32_t i = 0; i < 2; i++) {
				result.simd_[i] = left.simd_[i] + right.simd_[i];
			}
		}

		void Sub(Matrix2x3 &result, const Matrix2x3 &left, const Matrix2x3 &right) {
			for (uint32_t i = 0; i < 2; i++) {
				result.simd_[i] = left.simd_[i] - right.simd_[i];
			}
		}

		//void Mul(Matrix2x3 &result, const Matrix2x3 &left, const Matrix2x3 &right) {

		//}


	}

}

static Matrix2x3 operator+(const Matrix2x3 &l, const Matrix2x3 &r) {

	Matrix2x3 result;

	SoLib::Math::Add(result, l, r);
	return result;

}

//
//inline Vector3 operator*(const Vector3 &left, const Matrix3x3 &right)
//{
//	Vector3 result;
//
//	// 転置した行列
//	const Matrix3x3 tpMat = right.Transpose();
//
//	// ドット積で代入
//	for (uint32_t i = 0u; i < 3u; i++) {
//		result.arr()[i] = left * tpMat.vec[i];
//	}
//
//	return result;
//}

inline Vector3 operator*(const Vector3 &left, const Matrix3x3 &right)
{

	__m128 row0; *reinterpret_cast<Vector3 *>(&row0) = right.vec[0];
	__m128 row1; *reinterpret_cast<Vector3 *>(&row1) = right.vec[1];
	__m128 row2; *reinterpret_cast<Vector3 *>(&row2) = right.vec[2];

	__m128 xmm_all; *reinterpret_cast<Vector3 *>(&xmm_all) = left;

	// 1つの__m128構造体から4つの__m128に分解
	const __m128 brod0 = _mm_permute_ps(xmm_all, 0x00);
	const __m128 brod1 = _mm_permute_ps(xmm_all, 0x55);
	const __m128 brod2 = _mm_permute_ps(xmm_all, 0xAA);
	const __m128 result = { _mm_add_ps(
		_mm_add_ps(_mm_mul_ps(brod0, row0), _mm_mul_ps(brod1, row1)),
		_mm_mul_ps(brod2, row2)) };

	return *reinterpret_cast<const Vector3 *>(&result);
}

inline Vector3 &operator*=(Vector3 &left, const Matrix3x3 &right)
{

	__m128 row0; *reinterpret_cast<Vector3 *>(&row0) = right.vec[0];
	__m128 row1; *reinterpret_cast<Vector3 *>(&row1) = right.vec[1];
	__m128 row2; *reinterpret_cast<Vector3 *>(&row2) = right.vec[2];

	__m128 xmm_all; *reinterpret_cast<Vector3 *>(&xmm_all) = left;

	// 1つの__m128構造体から4つの__m128に分解
	const __m128 brod0 = _mm_permute_ps(xmm_all, 0x00);
	const __m128 brod1 = _mm_permute_ps(xmm_all, 0x55);
	const __m128 brod2 = _mm_permute_ps(xmm_all, 0xAA);
	const __m128 result = { _mm_add_ps(
		_mm_add_ps(_mm_mul_ps(brod0, row0), _mm_mul_ps(brod1, row1)),
		_mm_mul_ps(brod2, row2)) };

	return left = *reinterpret_cast<const Vector3 *>(&result);
}

inline Vector3 operator*(const Vector3 &left, const Quaternion &right) {
	return right.RotateVector(left);
}

inline Vector3 &operator*=(Vector3 &left, const Quaternion &right) {
	return left = right.RotateVector(left);
}

inline void Mul(Vector4 &result, const Vector4 &left, const Matrix4x4 &right)
{

	const __m128 row0 = _mm_loadu_ps(right.m[0].data());
	const __m128 row1 = _mm_loadu_ps(right.m[1].data());
	const __m128 row2 = _mm_loadu_ps(right.m[2].data());
	const __m128 row3 = _mm_loadu_ps(right.m[3].data());
	const __m128 xmm_all = _mm_loadu_ps(left.data());

	// 1つの__m128構造体から4つの__m128に分解
	const __m128 brod0 = _mm_permute_ps(xmm_all, 0x00);
	const __m128 brod1 = _mm_permute_ps(xmm_all, 0x55);
	const __m128 brod2 = _mm_permute_ps(xmm_all, 0xAA);
	const __m128 brod3 = _mm_permute_ps(xmm_all, 0xFF);
	// 計算結果を出す
	const __m128 row = _mm_add_ps(
		_mm_add_ps(_mm_mul_ps(brod0, row0), _mm_mul_ps(brod1, row1)),
		_mm_add_ps(_mm_mul_ps(brod2, row2), _mm_mul_ps(brod3, row3)));

	_mm_storeu_ps(result.data(), row);
}

inline Vector4 operator*(const Vector4 &left, const Matrix4x4 &right) {
	Vector4 result;

	Mul(result, left, right);

	return result;
}


inline Vector4 &operator*=(Vector4 &left, const Matrix4x4 &right) {

	Mul(left, left, right);

	return left;
}

