#pragma once

#include <numbers>
struct Polar;

struct Vector2;
struct Vector3;

struct Matrix2x2;
struct Matrix3x3;
struct Matrix4x4;

//static const Vector2 TextSize{ 50, 20 };



/// <summary>
/// 極座標系をベクトルに変換する。
/// </summary>
/// <param name="Polar">極座標</param>
/// <returns>ベクトル</returns>
Vector2 operator <<(Vector2 &vec2, const Polar &Polar);

/// <summary>
/// ベクトルを極座標系に変換する。
/// </summary>
/// <param name="Vector">ベクトル</param>
/// <returns>極座標クラス</returns>
Polar operator <<(Polar &Polar, const Vector2 &vec2);


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
//Vector2 Transform(const Vector2 &vector, const Matrix3x3 &matrix);
//
///// <summary>
///// ベクトルと同次座標系の合成
///// </summary>
///// <param name="vector">合成前のベクトル</param>
///// <param name="matrix">同次座標系の平行移動行列</param>
///// <returns>合成したベクトル</returns>
//Vector3 Transform(const Vector3 &vector, const Matrix4x4 &matrix);

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

float GetRandom(const float min, const float max);

namespace Angle {

	struct Digree {
		float digree;
		float ToRadian();
	};

	struct Radian {
		float radian;
		float ToDigree();
	};

	constexpr float PI = static_cast<float>(std::numbers::pi);
	constexpr float PI2 = PI * 2.f;

	constexpr float Dig2Rad = PI / 180.f;
	constexpr float Rad2Dig = 180.f / PI;

	float Lerp(const float start, const float end, const float t);
	Vector3 Lerp(const Vector3 &start, const Vector3 &end, const float t);

	float Mod(float radian);
	Vector3 Mod(const Vector3 &euler);

	//enum class 

} // namespace Angle