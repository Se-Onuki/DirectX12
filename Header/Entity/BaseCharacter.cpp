#include "BaseCharacter.h"

#include "../Model/Model.h"
#include "../../DirectBase/3D/ViewProjection/ViewProjection.h"

void BaseCharacter::Init(const std::unordered_map<std::string, Model *> &model) {
	modelMap_ = model;
	transformOrigin_.InitResource();
}

void BaseCharacter::Update() {
	transformOrigin_.UpdateMatrix();
}

void BaseCharacter::Draw(const ViewProjection &vp) const {
	for (auto &model : modelMap_) {
		model.second->Draw(transformOrigin_, vp);
	}
}
