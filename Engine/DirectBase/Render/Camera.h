#pragma once
#include <wrl.h>
#include <d3d12.h>

#include "../../../Utils/Math/Matrix4x4.h"
#include "../../../Utils/Math/Vector3.h"
#include "../../../Utils/Math/Math.hpp"

class Camera {
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	struct CameraMatrix {
		Matrix4x4 view;
		Matrix4x4 projection;
		Vector3 cameraPos;
	};

	Camera() = default;
	~Camera() = default;

	ComPtr<ID3D12Resource> constBuffer_ = nullptr;
	CameraMatrix *mapData_ = nullptr;	// マップ済みデータ

#pragma region ビュー行列の設定

	Vector3 rotation_ = { 0, 0, 0 };
	Vector3 translation_ = { 0, 0, -5 };

#pragma endregion

#pragma region 射影行列の設定
	// 垂直方向視野角
	float fovAngleY = 45.0f * Angle::Dig2Rad;
	// ビューポートのアスペクト比
	float aspectRatio = 16.f / 9;
	// 深度限界（手前側）
	float nearZ = 0.1f;
	// 深度限界（奥側）
	float farZ = 1000.0f;
#pragma endregion

	Matrix4x4 matView_{};
	Matrix4x4 matProjection_{};

	void Init();

	void CreateConstBuffer();

	void SetMap();

	void CalcMatrix();

	void UpdateMatrix();

	void TransferMatrix();

	bool ImGuiWidget();

};