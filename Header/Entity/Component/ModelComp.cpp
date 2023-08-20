#include "ModelComp.h"

void ModelComp::Update() {
	for (auto &[key, model] : modelMap_) {
		model.first.UpdateMatrix();
	}
}

void ModelComp::Draw(const ViewProjection &vp) const {
	for (auto &[key, model] : modelMap_) {
		model.second->Draw(model.first, vp);
	}
}

void ModelComp::SetModel(const std::unordered_map<std::string, std::pair<Transform, Model *>> &model) {
	modelMap_ = model;
}

void ModelComp::SetTransformParent() {
	const Transform *const parent = &object_->GetTransform();
	for (auto &[key, model] : modelMap_) {
		model.first.InitResource();
		model.first.parent_ = parent;
	}
}
