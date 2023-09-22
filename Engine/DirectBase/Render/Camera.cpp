#include "Camera.h"

#include "../Create/Create.h"
#include "../Base/DirectXCommon.h"
#include "../Render/Render.h"


void Camera::Init() {
	CreateConstBuffer();
	SetMap();
	UpdateMatrix();
}

void Camera::CreateConstBuffer()
{
	constBuffer_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(CameraMatrix));
}

void Camera::SetMap()
{
	HRESULT result = S_FALSE;
	result = constBuffer_->Map(0, nullptr, reinterpret_cast<void **>(&mapData_));
	assert(SUCCEEDED(result));
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
	mapData_->view = matView_;
	mapData_->projection = matProjection_;
	mapData_->cameraPos = translation_;
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
