#include "BaseCharacter.h"

#include "../../Engine/DirectBase/Model/Model.h"
#include "../../Engine/DirectBase/Render/Camera.h"

void BaseCharacter::Init(const std::unordered_map<std::string, Model *> &model) {
	modelMap_ = model;
}

void BaseCharacter::Update() { transformOrigin_->UpdateMatrix(); }

void BaseCharacter::Draw(const Camera<Render::CameraType::Projecction> &camera) const {
	for (auto &model : modelMap_) {
		model.second->Draw(transformOrigin_, camera);
	}
}
