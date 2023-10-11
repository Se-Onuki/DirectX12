#include "Transform.h"
#include "Matrix4x4.h"

#include "../../Engine/DirectBase/Create/Create.h"
#include "../../Engine/DirectBase/Base/DirectXCommon.h"

#include <imgui.h>
#include "Math.hpp"

//Transform::Transform(const Transform &other) {
//}

//Transform Transform::operator=(const Transform &other) noexcept {
//	other;
//	return Transform();
//}
//
//Transform Transform::operator=(Transform &&other) noexcept {
//	other;
//	return Transform{};
//}

Matrix4x4 Transform::Affine() const
{
	return Matrix4x4::Affine(scale, rotate, translate);
}

void Transform::InitResource()
{
	UpdateMatrix();
}

void Transform::CalcMatrix()
{
	matWorld_ = Affine();
	if (parent_) {
		matWorld_ *= parent_->matWorld_;
	}
}

void Transform::TransferMatrix() {
	mapBuffer_->World = matWorld_;
}

void Transform::UpdateMatrix()
{
	CalcMatrix();
	TransferMatrix();
}

bool Transform::ImGuiWidget()
{
	if (ImGui::TreeNode("Transform")) {
		bool isUsing = false;

		isUsing |= ImGui::DragFloat3("Scale", &scale.x, 0.01f, 0.001f, 100.f);

		isUsing |= ImGui::DragFloat3("Rotate", &rotate.x, Angle::Dig2Rad);

		isUsing |= ImGui::DragFloat3("Transform", &translate.x, 0.01f);

		ImGui::TreePop();
		return isUsing;
	}
	return false;
}

bool Transform::ImGuiWidget2D()
{
	if (ImGui::TreeNode("Transform2D")) {
		bool isUsing = false;


		isUsing |= ImGui::DragFloat2("Scale", &scale.x, 0.01f, 0.001f, 2048.f);

		isUsing |= ImGui::DragFloat("Rotate", &rotate.z, Angle::Dig2Rad);

		isUsing |= ImGui::DragFloat2("Transform", &translate.x, 0.01f, -100.f, 100.f);

		ImGui::TreePop();
		return isUsing;
	}
	return false;
}

void Transform::MatToSRT(const Matrix4x4 &mat) {
	// スケールの取得
	//scale.x = std::sqrt(mat.m[0][0] * mat.m[0][0] + mat.m[0][1] * mat.m[0][1] + mat.m[0][2] * mat.m[0][2]);
	scale.x = reinterpret_cast<const Vector3 *>(&mat.m[0])->Length();
	scale.y = reinterpret_cast<const Vector3 *>(&mat.m[1])->Length();
	scale.z = reinterpret_cast<const Vector3 *>(&mat.m[2])->Length();

	/*
	scale.x = std::sqrt(_mm_cvtss_f32(_mm_dp_ps(*(__m128 *)mat.m[0], *(__m128 *) mat.m[0], 0x71)));
	scale.y = std::sqrt(_mm_cvtss_f32(_mm_dp_ps(*(__m128 *)mat.m[1], *(__m128 *) mat.m[1], 0x71)));
	scale.z = std::sqrt(_mm_cvtss_f32(_mm_dp_ps(*(__m128 *)mat.m[2], *(__m128 *) mat.m[2], 0x71)));
	*/

	// 回転行列の取得
	Matrix4x4 rotMat;
	*(__m128 *)rotMat.m[0] = _mm_div_ps(_mm_load_ps(mat.m[0]), _mm_set1_ps(scale.x));
	*(__m128 *)rotMat.m[1] = _mm_div_ps(_mm_load_ps(mat.m[1]), _mm_set1_ps(scale.y));
	*(__m128 *)rotMat.m[2] = _mm_div_ps(_mm_load_ps(mat.m[2]), _mm_set1_ps(scale.z));


	// 回転角度の取得
	rotate.x = std::atan2(rotMat.m[1][2], rotMat.m[2][2]);
	rotate.y = std::atan2(-rotMat.m[0][2], std::sqrt(rotMat.m[1][2] * rotMat.m[1][2] + rotMat.m[2][2] * rotMat.m[2][2]));
	rotate.z = std::atan2(rotMat.m[0][1], rotMat.m[0][0]);

	// 移動量の取得
	translate = *(Vector3 *)mat.m[3];
}

void Transform::SetParent(const Transform &parent) {
	parent_ = &parent;
}

void Transform::ConnectParent(const Transform &parent) {
	// 親子関係を解除
	DisConnectParent();
	// 接続する親の逆行列でローカル座標行列を算出
	Matrix4x4 mat{ matWorld_ * parent.matWorld_.InverseSRT() };

	// ローカル座標行列からSRTを適用
	this->MatToSRT(mat);
	// 親を設定
	this->SetParent(parent);
	// 座標更新
	this->CalcMatrix();
}

void Transform::DisConnectParent() {
	if (parent_ == nullptr) { return; }
	this->CalcMatrix();

	this->MatToSRT(matWorld_);
	parent_ = nullptr;

	this->CalcMatrix();
}
