#include "Ground.h"
#include "../Model/ModelManager.h"

void Ground::Init() {
	model_ = ModelManager::GetInstance()->GetModel("ground");
	transform_.InitResource();
	transform_.scale *= 500.f;
}

void Ground::Update() {
	transform_.UpdateMatrix();
}

void Ground::Draw(const ViewProjection &Vp) const {
	model_->Draw(transform_, Vp);
}