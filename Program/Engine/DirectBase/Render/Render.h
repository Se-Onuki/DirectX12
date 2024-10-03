#pragma once
#include "../../Engine/Utils/Math/Vector2.h"
#include "../../Engine/Utils/Math/Vector3.h"
#include "../../Engine/Utils/Math/Vector4.h"
#include "../../Engine/Utils/Math/Matrix4x4.h"
#include "../../Engine/Utils/Math/Math.hpp"

struct Vector2;
struct Matrix4x4;

class Render {
public:
	enum class CameraType : uint32_t {
		Projecction,	// 透視投影行列
		Othographic		// 正射影行列
	};

	/// @brief 正射影行列関数
	/// @param LeftTop 左上
	/// @param RightBottom 右下
	/// @param nearClip 近平面
	/// @param farClip 遠平面
	/// @return 正射影行列
	static Matrix4x4 MakeOrthographicMatrix(
		const Vector2 &LeftTop, const Vector2 &RightBottom, const float &nearClip,
		const float &farClip);

	/// @brief 透視射影行列関数
	/// @param fovY 縦画角
	/// @param aspectRatio アスペクト比
	/// @param nearClip 近平面
	/// @param farClip 遠平面
	/// @return 透視射影行列
	static Matrix4x4 MakePerspectiveFovMatrix(
		const float &fovY, const float &aspectRatio, const float &nearClip, const float &farClip);

	static Matrix4x4 MakeViewportMatrix(
		const Vector2 &LeftTop, const float &width, const float &height, const float &minDepth = 0.f,
		const float &maxDepth = 1.f);

	/// @brief スクリーン座標から3D座標に変換する
	/// @param screenPos 2次元ベクトル
	/// @param matVPVp 座標変換行列
	/// @return 始点と終点座標 { 近, 遠 }
	static std::pair<Vector3, Vector3> ScreenToWorld(const Vector2 &screenPos, const Matrix4x4 &matVPVp);

	/// @brief 3D座標からスクリーン座標に変換する
	/// @param worldPos 3次元の座標
	/// @param matVPVp 座標変換行列
	/// @return スクリーン上の座標
	static Vector3 WorldToScreen(const Vector3 &worldPos, const Matrix4x4 &matVPVp);

	static float GetObjectScale(const Vector3 &worldPos, const Matrix4x4 &projectionMatrix) {

		Vector4 pos = { .w = 1.f };
		pos.ToVec3() = worldPos;

		// 座標変換
		Vector4 transformedPosition = pos * projectionMatrix;

		// プロジェクション変換後の座標を除算してスケールを取得
		return  transformedPosition.w != 0.0f ? transformedPosition.w : 1.0f;

	}
};
//
//namespace Light {
//	struct Direction {
//		Vector4 color;		// 色(RGBA)
//		Vector3 direction;	// ライトの向き
//		float intensity;	// 輝度
//	};
//}