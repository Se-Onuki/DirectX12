#include "Skydome.h"
#include "../../Engine/DirectBase/Model/ModelManager.h"

Skydome::Skydome() {}

Skydome::~Skydome() {}

void Skydome::Init() {
	model_ = ModelManager::GetInstance()->GetModel("skydome");
	model_->materialMap_["sky"]->materialBuff_->emissive = Vector4{ 1.f,1.f,1.f,1.f };
	transform_->scale *= 500.f;
	transform_->UpdateMatrix();
}

void Skydome::Update() { transform_->UpdateMatrix(); }

void Skydome::Draw(const Camera<Render::CameraType::Projecction> &camera) const { model_->Draw(transform_, camera); }