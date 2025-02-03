/// @file Render.cpp
/// @brief レンダラー
/// @author ONUKI seiya
#include "Render.h"

#include <cmath>

namespace SolEngine {
	Matrix4x4 Render::MakeOrthographicMatrix(
		const Vector2 LeftTop, const Vector2 RightBottom, const float &nearClip,
		const float &farClip) {
		return Matrix4x4{
			2.f / (RightBottom.x - LeftTop.x), 0.f, 0.f, 0.f,

			0.f, 2.f / (LeftTop.y - RightBottom.y), 0.f, 0.f,

			0.f, 0.f, 1 / (farClip - nearClip), 0.f,

			(RightBottom.x + LeftTop.x) / (LeftTop.x - RightBottom.x),
			(LeftTop.y + RightBottom.y) / (RightBottom.y - LeftTop.y),
			nearClip / (nearClip - farClip),
			1.f
		};
	}

	Matrix4x4 Render::MakePerspectiveFovMatrix(
		const float &fovY, const float &aspectRatio, const float &nearClip, const float &farClip) {
		return Matrix4x4{
			(1 / aspectRatio) * (1 / std::tan(fovY / 2)), 0, 0, 0,
			0,(1 / std::tan(fovY / 2)),0,0,
			0, 0, farClip / (farClip - nearClip),1,
			0, 0, (-nearClip * farClip) / (farClip - nearClip),0
		};
	}

	Matrix4x4 Render::MakeViewportMatrix(
		const Vector2 LeftTop, const float &width, const float &height, const float &minDepth,
		const float &maxDepth) {
		return Matrix4x4{
			width / 2, 0, 0, 0,
			0, -height / 2, 0, 0,
			0, 0, maxDepth - minDepth, 0,
			LeftTop.x + width / 2, LeftTop.y + height / 2, minDepth, 1
		};
	}



	std::pair<Vector3, Vector3> Render::ScreenToWorld(const Vector2 screenPos, const Matrix4x4 &matVPVp) {
		Matrix4x4 matInvarseVPVp = matVPVp.InverseSRT();

		Vector3 posNear{ screenPos.x, screenPos.y, 0.f };
		Vector3 posFar{ screenPos.x, screenPos.y, 1.f };

		posNear = posNear * matInvarseVPVp;
		posFar = posFar * matInvarseVPVp;

		std::pair<Vector3, Vector3> segment;
		segment.first = posNear;
		segment.second = posFar;
		return segment;
	}

	Vector3 Render::WorldToScreen(const Vector3 &worldPos, const Matrix4x4 &matVPVp) {
		return worldPos * matVPVp;
	}
}