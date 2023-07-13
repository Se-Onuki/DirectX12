#include "Transform.h"
#include "Matrix4x4.h"

#include "../Create/Create.h"
#include "../../DirectBase/Base/DirectXCommon.h"

Matrix4x4 Transform::Affine() const
{
	return Matrix4x4::Affine(scale, rotate, translate);
}

void Transform::Init()
{
	CreateConstBuffer();
	SetMap();
	UpdateMatrix();
}

void Transform::CreateConstBuffer()
{
	constBuffer_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(TransformMatrix));
}

void Transform::SetMap()
{
	HRESULT result = constBuffer_->Map(0, nullptr, reinterpret_cast<void **>(&mapData_));
	assert(SUCCEEDED(result));
}

void Transform::UpdateMatrix()
{
	matWorld_ = Affine();
	if (parent_) {
		matWorld_ *= parent_->matWorld_;
	}
	mapData_->World = matWorld_;
}
