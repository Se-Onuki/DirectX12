#include "Camera.h"

#include "../Create/Create.h"
#include "../Base/DirectXCommon.h"

#include "../Render/Render.h"
#include "../Base/WinApp.h"


template<>
void Camera::CalcMatrix<Camera::Type::Projecction>() {
	matView_ = Matrix4x4::Affine(Vector3::one, rotation_, translation_).InverseSRT();
	matProjection_ = Render::MakePerspectiveFovMatrix(fovAngleY, aspectRatio, nearZ, farZ);
}

template<>
void Camera::CalcMatrix<Camera::Type::Othographic>() {
	matView_ = Matrix4x4::Affine(Vector3::one, rotation_, translation_).InverseSRT();
	Vector2 windowSize = { (float)WinApp::kWindowWidth,(float)WinApp::kWindowHeight };
	matProjection_ = Render::MakeOrthographicMatrix(windowSize * -.5f, windowSize * +.5f, 0.f, 100.f);
}

void Camera::Init() {
	UpdateMatrix();
}


void Camera::CalcMatrix() {
	switch (cameraType_) {
	case Camera::Type::Projecction:
		CalcMatrix<Camera::Type::Projecction>();
		break;
	case Camera::Type::Othographic:
		CalcMatrix<Camera::Type::Othographic>();
		break;
	}
}

void Camera::UpdateMatrix() {
	CalcMatrix();
	TransferMatrix();
}

void Camera::TransferMatrix() {
	constData_->view = matView_;
	constData_->projection = matProjection_;
	constData_->cameraPos = translation_;
}

bool Camera::ImGuiWidget() {

	if (ImGui::TreeNode("Camera")) {
		bool isUsing = false;

		isUsing |= ImGui::DragFloat3("Rotate", &rotation_.x, Angle::Dig2Rad);

		isUsing |= ImGui::DragFloat3("Transform", &translation_.x, 0.01f);

		ImGui::TreePop();
		return isUsing;
	}
	return false;
}
