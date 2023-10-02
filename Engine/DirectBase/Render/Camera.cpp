#include "Camera.h"

#include "../Create/Create.h"
#include "../Base/DirectXCommon.h"

#include "../Render/Render.h"


void Camera<Render::CameraType::Projecction>::Init() {
	UpdateMatrix();
}

void Camera<Render::CameraType::Projecction>::UpdateMatrix() {
	CalcMatrix();
	TransferMatrix();
}

void Camera<Render::CameraType::Projecction>::TransferMatrix() {
	constData_->view = matView_;
	constData_->projection = matProjection_;
	constData_->cameraPos = translation_;
}

bool Camera<Render::CameraType::Projecction>::ImGuiWidget() {

	if (ImGui::TreeNode("Camera")) {
		bool isUsing = false;

		isUsing |= ImGui::DragFloat3("Rotate", &rotation_.x, Angle::Dig2Rad);

		isUsing |= ImGui::DragFloat3("Transform", &translation_.x, 0.01f);

		ImGui::TreePop();
		return isUsing;
	}
	return false;
}

void Camera<Render::CameraType::Projecction>::CalcMatrix() {
	matView_ = Matrix4x4::Affine(Vector3::one, rotation_, translation_).InverseSRT();
	matProjection_ = Render::MakePerspectiveFovMatrix(fovAngleY, aspectRatio, nearZ, farZ);
}



void Camera<Render::CameraType::Othographic>::Init() {
	UpdateMatrix();
}

void Camera<Render::CameraType::Othographic>::UpdateMatrix() {
	CalcMatrix();
	TransferMatrix();
}

void Camera<Render::CameraType::Othographic>::TransferMatrix() {
	constData_->view = matView_;
	constData_->projection = matProjection_;
	constData_->cameraPos = translation_;
}

bool Camera<Render::CameraType::Othographic>::ImGuiWidget() {

	if (ImGui::TreeNode("Camera")) {
		bool isUsing = false;

		isUsing |= ImGui::DragFloat("Rotate", &rotation_.z, Angle::Dig2Rad);

		isUsing |= ImGui::DragFloat2("Transform", &translation_.x, 0.01f);

		ImGui::TreePop();
		return isUsing;
	}
	return false;
}


void Camera<Render::CameraType::Othographic>::CalcMatrix() {
	matView_ = Matrix4x4::Affine(Vector3::one, rotation_, translation_).InverseSRT();

	matProjection_ = Render::MakeOrthographicMatrix(windowSize_ * -.5f, windowSize_ * +.5f, 0.f, 100.f);
}