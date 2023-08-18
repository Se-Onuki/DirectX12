#pragma once
#include <wrl.h>
#include <d3d12.h>

#include "../../../Header/Math/Matrix4x4.h"
#include "../../../Header/Math/Vector3.h"
#include "../../../Header/Math/Math.hpp"

class ViewProjection
{
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	struct ViewProjectionMatrix {
		Matrix4x4 view;
		Matrix4x4 projection;
		Vector3 cameraPos;
	};

	ViewProjection();
	~ViewProjection();

	ComPtr<ID3D12Resource> constBuffer_ = nullptr;
	ViewProjectionMatrix *mapData_ = nullptr;	// マップ済みデータ

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

	Matrix4x4 matView_;
	Matrix4x4 matProjection_;

	void Init();

	void CreateConstBuffer();

	void SetMap();

	void UpdateMatrix();

	bool ImGuiWidget();

};