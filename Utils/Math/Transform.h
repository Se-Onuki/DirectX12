#pragma once
#include "Vector3.h"
#include "Matrix4x4.h"
#include "Math.hpp"

#include <d3d12.h>
#include <wrl.h>
#include "../../Engine/DirectBase/Base/CBuffer.h"

template <bool IsBufferActive>
struct BaseTransform {
	BaseTransform &operator=(const BaseTransform &other) = default;
	//Transform &operator=(Transform &&other) = default;

	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	struct TransformMatrix {
		Matrix4x4 World;
	};


	Vector3 scale = { 1.f,1.f,1.f };
	Vector3 rotate{};
	Vector3 translate{};

	Matrix4x4 matWorld_{};	// ローカル . ワールド変換
	const BaseTransform *parent_ = nullptr;	// 親へのアドレス

	//ComPtr<ID3D12Resource> constBuffer_ = nullptr;
	//TransformMatrix *mapData_ = nullptr;	// マップ済みデータ

	CBuffer<TransformMatrix, IsBufferActive> mapBuffer_;

	inline Matrix4x4 Affine() const;

	inline void CalcMatrix();
	inline void TransferMatrix();
	inline void UpdateMatrix();

	inline bool ImGuiWidget();
	inline bool ImGuiWidget2D();

	inline void Create(const Matrix4x4 &mat);
private:
};


template <bool IsBufferActive>
Matrix4x4 BaseTransform<IsBufferActive>::Affine() const
{
	return Matrix4x4::Affine(scale, rotate, translate);
}


template <bool IsBufferActive>
void BaseTransform<IsBufferActive>::CalcMatrix()
{
	matWorld_ = Affine();
	if (parent_) {
		matWorld_ *= parent_->matWorld_;
	}
}

template <bool IsBufferActive>
void BaseTransform<IsBufferActive>::TransferMatrix() {
	if (mapBuffer_) {
		mapBuffer_->World = matWorld_;
	}
}

template <bool IsBufferActive>
void BaseTransform<IsBufferActive>::UpdateMatrix()
{
	CalcMatrix();
	TransferMatrix();
}

template <bool IsBufferActive>
bool BaseTransform<IsBufferActive>::ImGuiWidget()
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

template <bool IsBufferActive>
bool BaseTransform<IsBufferActive>::ImGuiWidget2D()
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

template <bool IsBufferActive>
void BaseTransform<IsBufferActive>::Create(const Matrix4x4 &mat) {
	// スケールの取得
	//scale.x = std::sqrt(mat.m[0][0] * mat.m[0][0] + mat.m[0][1] * mat.m[0][1] + mat.m[0][2] * mat.m[0][2]);
	scale.x = std::sqrt(_mm_cvtss_f32(_mm_dp_ps(*(__m128 *)mat.m[0], *(__m128 *) mat.m[0], 0x71)));
	scale.y = std::sqrt(_mm_cvtss_f32(_mm_dp_ps(*(__m128 *)mat.m[1], *(__m128 *) mat.m[1], 0x71)));
	scale.z = std::sqrt(_mm_cvtss_f32(_mm_dp_ps(*(__m128 *)mat.m[2], *(__m128 *) mat.m[2], 0x71)));

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

using Transform = BaseTransform<true>;