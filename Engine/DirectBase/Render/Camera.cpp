#include "Camera.h"

#include "../Create/Create.h"
#include "../Base/DirectXCommon.h"

#include "../Render/Render.h"
#include "../Base/WinApp.h"


template<>
void Camera<Render::CameraType::Projecction>::CalcMatrix() {
	matView_ = Matrix4x4::Affine(Vector3::one, rotation_, translation_).InverseSRT();
	matProjection_ = Render::MakePerspectiveFovMatrix(fovAngleY, aspectRatio, nearZ, farZ);
}

//template<>
//void Camera<Render::CameraType::Othographic>::CalcMatrix() {
//	matView_ = Matrix4x4::Affine(Vector3::one, rotation_, translation_).InverseSRT();
//	Vector2 windowSize = { (float)WinApp::kWindowWidth,(float)WinApp::kWindowHeight };
//	matProjection_ = Render::MakeOrthographicMatrix(windowSize * -.5f, windowSize * +.5f, 0.f, 100.f);
//}

template<>
void Camera<Render::CameraType::Projecction>::Init() {
	UpdateMatrix();
}

template<>
void Camera<Render::CameraType::Projecction>::UpdateMatrix() {
	CalcMatrix();
	TransferMatrix();
}
template<>
void Camera<Render::CameraType::Projecction>::TransferMatrix() {
	constData_->view = matView_;
	constData_->projection = matProjection_;
	constData_->cameraPos = translation_;
}
template<>
void Camera<Render::CameraType::Projecction>::ImGuiWidget() {

	if (ImGui::TreeNode("Camera")) {
		bool isUsing = false;

		isUsing |= ImGui::DragFloat3("Rotate", &rotation_.x, Angle::Dig2Rad);

		isUsing |= ImGui::DragFloat3("Transform", &translation_.x, 0.01f);

		ImGui::TreePop();
		return isUsing;
	}
	return false;
}
