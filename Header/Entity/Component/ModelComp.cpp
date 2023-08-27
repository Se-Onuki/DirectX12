#include "ModelComp.h"

void ModelComp::Update() {
	for (auto &[key, model] : modelMap_) {
		model.first.CalcMatrix();
	}
	for (auto &[key, model] : modelMap_) {
		model.first.TransferMatrix();
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

		model.first.InitResource();

		if (model.first.parent_ == nullptr) {
			model.first.parent_ = parent;
		}
	}
}

void ModelComp::ModelTree::Init() {
	transform_.InitResource();
}

void ModelComp::ModelTree::AddChild(Model *const model) {
	ModelTree *const modelTree = new ModelTree;
	modelTree->Init();

	modelTree->model_ = model;
	modelTree->SetParent(this);

	children_.emplace_back(modelTree);
}

void ModelComp::ModelTree::SetParent(ModelTree *parent) {
	parent_ = parent;
	transform_.parent_ = &parent->transform_;
}
