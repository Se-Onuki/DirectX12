/// @file Camera.h
/// @brief カメラ
/// @author ONUKI seiya
#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <stdint.h>
#include <assert.h>

#include "Render.h"

#include "../../Engine/Utils/Math/Matrix4x4.h"
#include "../../Engine/Utils/Math/Vector3.h"
#include "../../Engine/Utils/Math/Math.hpp"
#include "../Base/CBuffer.h"

#include "../Base/WinApp.h"

namespace SolEngine {

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
		Camera(const Camera &) = default;
		~Camera() = default;

		CBuffer<CameraMatrix> constData_;

#pragma region ビュー行列の設定

		Quaternion rotation_ = Quaternion::Identity;
		Vector3 translation_ = { 0, 0, -5 };

#pragma endregion

#pragma region 射影行列の設定
		// 垂直方向視野角
		float fovAngleY = 45.0f * SoLib::Angle::Dig2Rad;
		// ビューポートのアスペクト比
		float aspectRatio = 16.f / 9;
		// 深度限界（手前側）
		float nearZ = 0.1f;
		// 深度限界（奥側）
		float farZ = 1000.0f;
#pragma endregion

		Matrix4x4 matView_{};
		Matrix4x4 matProjection_{};

		/// @brief 初期化
		void Init();

		/// @brief 行列の計算
		void CalcMatrix();

		/// @brief 行列の更新
		/// @details ビュー行列と射影行列を計算して､転送する
		void UpdateMatrix();

		/// @brief 行列の転送
		void TransferMatrix();

		/// @brief ImGui表示
		/// @return 変更されたらtrue
		bool ImGuiWidget();

		/// @brief ImGui表示
		/// @param[in] id 表示名
		/// @return 変更されたらtrue
		bool ImGuiWidget(const ::std::string &id);

	};

	/// @brief 正射影投影カメラ
	template<>
	class Camera<Render::CameraType::Othographic> {
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

		// 画面サイズ
		Vector2 windowSize_ = { (float)WinApp::kWindowWidth,(float)WinApp::kWindowHeight };
		// 深度限界（手前側）
		float nearZ = 0.f;
		// 深度限界（奥側）
		float farZ = 100.0f;
#pragma endregion

		Matrix4x4 matView_{};
		Matrix4x4 matProjection_{};

		/// @brief 初期化
		void Init();

		/// @brief 行列の計算
		void CalcMatrix();

		/// @brief 行列の更新
		/// @details ビュー行列と射影行列を計算して､転送する
		void UpdateMatrix();

		/// @brief 行列の転送
		void TransferMatrix();

		/// @brief ImGui表示
		/// @return 変更されたらtrue
		bool ImGuiWidget();

	};

	/// @brief 3次元カメラ
	using Camera3D = Camera<Render::CameraType::Projecction>;

}