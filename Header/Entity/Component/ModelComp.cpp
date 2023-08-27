#include "ModelComp.h"

void ModelComp::Update() {
	for (auto &model : modelTree_) {
		model->Update();
	}
}

void ModelComp::Draw(const ViewProjection &vp) const {
	for (auto &model : modelTree_) {
		model->Draw(vp);
	}
}

void ModelComp::ModelBone::Init(Model *const model) {
	transform_.InitResource();
	if (model) { model_ = model; }
}

void ModelComp::ModelBone::SetTransform(const Transform &srt) {
	transform_.scale = srt.scale;
	transform_.rotate = srt.rotate;
	transform_.translate = srt.translate;
}

ModelComp::ModelBone *const ModelComp::ModelBone::AddChild(Model *const model) {
	ModelBone *const modelBone = new ModelBone;
	modelBone->Init(model);

	AddChild(modelBone);
	return modelBone;
}

void ModelComp::ModelBone::AddChild(ModelBone *const child) {
	child->SetParent(this);
	children_.emplace_back(child);
}

void ModelComp::ModelBone::SetParent(ModelBone *const parent) {
	parent_ = parent;
	transform_.parent_ = &parent->transform_;
}

void ModelComp::ModelBone::Update() {

	transform_.UpdateMatrix();

	for (auto &child : children_) {
		child->Update();
	}
}

void ModelComp::ModelBone::Draw(const ViewProjection &vp) const {

	this->model_->Draw(transform_, vp);

	for (auto &child : children_) {
		child->Draw(vp);
	}
}

ModelComp::ModelBone *const ModelComp::AddBone(const std::string &key, Model *const model, const Transform &srt) {
	if (modelKey_.count(key) != 0u) return nullptr;
	auto *const newBone = new ModelBone;
	newBone->Init(model);

	newBone->transform_.parent_ = &object_->transform_;

	newBone->SetTransform(srt);

	modelTree_.emplace_back(newBone);

	modelKey_.insert({ key,newBone });

	return newBone;
}

ModelComp::ModelBone *const ModelComp::AddBone(const std::string &key, Model *const model, ModelBone *const parent, const Transform &srt) {
	if (modelKey_.count(key) != 0u) return nullptr;

	auto *const newBone = parent->AddChild(model);

	newBone->SetTransform(srt);

	modelKey_.insert({ key,newBone });

	return newBone;
}
