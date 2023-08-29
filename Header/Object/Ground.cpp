#include "Ground.h"
#include "../Model/ModelManager.h"

float Ground::stageRadius_ = 200.f;

void Ground::Init() {
	model_ = ModelManager::GetInstance()->GetModel("ground");
	transform_.InitResource();
	SetStageRadius(stageRadius_);
	transform_.UpdateMatrix();
}

void Ground::Update() {
	transform_.UpdateMatrix();
}

void Ground::Draw(const ViewProjection &Vp) const {
	model_->Draw(transform_, Vp);
}

void Ground::SetStageRadius(const float value) {
	stageRadius_ = value;
	transform_.scale = Vector3::one() * value;
	transform_.translate = Vector3{ 0.f,-1.f,0.f } *value;
}
