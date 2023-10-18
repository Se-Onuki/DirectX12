#include "BaseCharacter.h"

#include "../../Engine/DirectBase/Model/Model.h"
#include "../../Engine/DirectBase/Render/Camera.h"

void BaseCharacter::Init(const std::unordered_map<std::string, Model *> &model) {
	modelMap_ = model;
	transformOrigin_.InitResource();
}

void BaseCharacter::Update([[maybe_unused]] const float deltaTime) { transformOrigin_.UpdateMatrix(); }

void BaseCharacter::Draw(const Camera<Render::CameraType::Projecction> &camera) const {
	for (auto &model : modelMap_) {
		model.second->Draw(transformOrigin_, camera);
	}
}
