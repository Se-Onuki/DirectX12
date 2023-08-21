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

void ModelComp::SetModel(const ModelMap &model) {
	modelMap_ = model;
	SetTransformParent();
}

void ModelComp::SetTransformParent() {
	const Transform *const parent = &object_->transform_;
	for (auto &[key, model] : modelMap_) {
		if (model.first.constBuffer_ == nullptr) {
			model.first.InitResource();
		}
		if (model.first.parent_ == nullptr) {
			model.first.parent_ = parent;
		}
	}
}
