#include "Ground.h"
#include "../Model/ModelManager.h"

void Ground::Init() {
	model_ = ModelManager::GetInstance()->GetModel("ground");
	transform_.InitResource();
	SetStageRadius(stageRadius_);
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
}
