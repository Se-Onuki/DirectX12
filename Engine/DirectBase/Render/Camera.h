#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <stdint.h>
#include <assert.h>

#include "../../../Utils/Math/Matrix4x4.h"
#include "../../../Utils/Math/Vector3.h"
#include "../../../Utils/Math/Math.hpp"

class Camera {
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	enum class Type : uint32_t {
		Projecction,	// 透視投影行列
		Othographic		// 正射影行列
	};

	struct CameraMatrix {
		Matrix4x4 view;
		Matrix4x4 projection;
		Vector3 cameraPos;
	};

	Camera(Camera::Type type) :cameraType_(type) {}
	~Camera() = default;

	ComPtr<ID3D12Resource> constBuffer_ = nullptr;
	CameraMatrix *mapData_ = nullptr;	// マップ済みデータ

	const Type cameraType_;

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

	/// @brief 前方宣言
	/// @tparam T カメラタイプ
	template<Camera::Type T>
	void CalcMatrix();

	/// @brief 透視投影計算
	template<>
	void CalcMatrix<Camera::Type::Projecction>();

	/// @brief 正射影計算
	template<>
	void CalcMatrix<Camera::Type::Othographic>();

	void UpdateMatrix();

	template<Camera::Type T>
	void UpdateMatrix();

	void TransferMatrix();

	bool ImGuiWidget();

};

template<Camera::Type T>
void Camera::UpdateMatrix() {
	assert(cameraType_ == T && "入力した値とカメラ設定が一致しませんでした");
	CalcMatrix<T>();
	TransferMatrix();
}