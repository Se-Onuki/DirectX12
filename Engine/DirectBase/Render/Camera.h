#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <stdint.h>
#include <assert.h>

#include "Render.h"

#include "../../../Utils/Math/Matrix4x4.h"
#include "../../../Utils/Math/Vector3.h"
#include "../../../Utils/Math/Math.hpp"
#include "../Base/CBuffer.h"

template <Render::CameraType T>
class Camera;

/// @brief 透視投影カメラ
template<>
class Camera<Render::CameraType::Projecction> {
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:

	struct CameraMatrix {
		Matrix4x4 view;
		Matrix4x4 projection;
		Vector3 cameraPos;
	};

	Camera() = default;
	~Camera() = default;

	CBuffer<CameraMatrix> constData_;

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

	void Init() { UpdateMatrix(); }

	void CalcMatrix() {
		matView_ = Matrix4x4::Affine(Vector3::one, rotation_, translation_).InverseSRT();
		matProjection_ = Render::MakePerspectiveFovMatrix(fovAngleY, aspectRatio, nearZ, farZ);
	}

	void UpdateMatrix() {
		CalcMatrix();
		TransferMatrix();
	}

	void TransferMatrix() {
		constData_->view = matView_;
		constData_->projection = matProjection_;
		constData_->cameraPos = translation_;
	}

	bool ImGuiWidget() {

		if (ImGui::TreeNode("Camera")) {
			bool isUsing = false;

			isUsing |= ImGui::DragFloat3("Rotate", &rotation_.x, Angle::Dig2Rad);

			isUsing |= ImGui::DragFloat3("Transform", &translation_.x, 0.01f);

			ImGui::TreePop();
			return isUsing;
		}
		return false;
	}


};
//
///// @brief 正射影投影カメラ
//template<>
//class Camera<Render::CameraType::Othographic> {
//	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
//
//public:
//
//	struct CameraMatrix {
//		Matrix4x4 view;
//		Matrix4x4 projection;
//		Vector3 cameraPos;
//	};
//
//	Camera() = default;
//	~Camera() = default;
//
//	CBuffer<CameraMatrix> constData_;
//
//#pragma region ビュー行列の設定
//
//	Vector3 rotation_ = { 0, 0, 0 };
//	Vector3 translation_ = { 0, 0, -5 };
//
//#pragma endregion
//
//#pragma region 射影行列の設定
//	// 垂直方向視野角
//	float fovAngleY = 45.0f * Angle::Dig2Rad;
//	// ビューポートのアスペクト比
//	float aspectRatio = 16.f / 9;
//	// 深度限界（手前側）
//	float nearZ = 0.1f;
//	// 深度限界（奥側）
//	float farZ = 1000.0f;
//#pragma endregion
//
//	Matrix4x4 matView_{};
//	Matrix4x4 matProjection_{};
//
//	void Init();
//
//	void CalcMatrix();
//
//	/// @brief 前方宣言
//	/// @tparam T カメラタイプ
//	template<Render::CameraType T>
//	void CalcMatrix();
//
//	/// @brief 透視投影計算
//	template<>
//	void CalcMatrix<Render::CameraType::Projecction>();
//
//	/// @brief 正射影計算
//	template<>
//	void CalcMatrix<Render::CameraType::Othographic>();
//
//	void UpdateMatrix();
//
//	template<Render::CameraType T>
//	void UpdateMatrix();
//
//	void TransferMatrix();
//
//	bool ImGuiWidget();
//
//};