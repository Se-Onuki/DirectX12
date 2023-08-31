#include "SkyCylinder.h"
#include "../Model/ModelManager.h"

float SkyCylinder::radius_ = 1200.f;

void SkyCylinder::Init() {
	model_ = ModelManager::GetInstance()->GetModel("skyCylinder");
	model_->materialMap_["sky"]->mapData_->emissive = Vector4{ 1.f,1.f,1.f,1.f };
	transform_.InitResource();
	SetRadius(radius_);

	transform_.UpdateMatrix();
}

void SkyCylinder::Update() {
	transform_.UpdateMatrix();
}

void SkyCylinder::Draw(const ViewProjection &Vp) const {
	model_->Draw(transform_, Vp);
}

void SkyCylinder::SetRadius(const float value) {
	radius_ = value;
	transform_.scale = Vector3{ value, value * 0.5f,value };
	transform_.translate = Vector3::up() * value * 0.25f;
}
