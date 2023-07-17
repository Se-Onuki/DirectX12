#pragma once
#include "../Math/Vector2.h"
#include "../Math/Vector3.h"
#include "../Math/Vector4.h"
#include "../Math/Matrix4x4.h"
struct Vector2;
struct Matrix4x4;

class Render {
public:

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
		const Vector2 &LeftTop, const float &width, const float &height, const float &minDepth,
		const float &maxDepth);
};

namespace Light {
	struct Direction {
		Vector4 color;		// 色(RGBA)
		Vector3 direction;	// ライトの向き
		float intensity;	// 輝度
	};
}