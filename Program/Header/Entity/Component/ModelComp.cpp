#include "ModelComp.h"

void ModelComp::Update() {
	for (auto &model : modelTree_) {
		model->Update();
	}
}

void ModelComp::Draw(const Camera<Render::CameraType::Projecction> &vp) const {
	for (auto &model : modelTree_) {
		model->Draw(vp);
	}
}

void ModelComp::Draw(const Camera<Render::CameraType::Projecction> &vp, const Material &material) const {
	for (auto &model : modelTree_) {
		model->Draw(vp, material);
	}
}

void ModelComp::ImGuiWidget() {
	for (auto &item : modelTree_) {
		item->ImGuiWidget();
	}
}

void ModelComp::ModelBone::Init(Model *const model) {
	if (model) { model_ = model; }
}

void ModelComp::ModelBone::SetTransform(const BaseTransform &srt) {
	transform_->scale = srt.scale;
	transform_->rotate = srt.rotate;
	transform_->translate = srt.translate;
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
	transform_->parent_ = &parent->transform_;
}

void ModelComp::ModelBone::Update() {

	transform_->UpdateMatrix();

	for (auto &child : children_) {
		child->Update();
	}
}

void ModelComp::ModelBone::Draw(const Camera<Render::CameraType::Projecction> &vp) const {
	if (model_) {
		this->model_->Draw(transform_, vp);
	}
	for (auto &child : children_) {
		child->Draw(vp);
	}
}

void ModelComp::ModelBone::Draw(const Camera<Render::CameraType::Projecction> &vp, const Material &material) const {
	this->model_->Draw(transform_, vp);

	for (auto &child : children_) {
		child->Draw(vp, material);
	}
}

bool ModelComp::ModelBone::ImGuiWidget() {
	bool result = false;
	result |= this->model_->ImGuiWidget();

	result |= this->transform_->ImGuiWidget(model_->name_);

	//for (auto &item : this->children_) {
	//	result |= item->ImGuiWidget();
	//}
	return result;
}

ModelComp::ModelBone *const ModelComp::AddBone(const std::string &key, Model *const model, const BaseTransform &srt) {
	if (modelKey_.count(key) != 0u) return nullptr;
	auto  newBone = std::make_unique<ModelBone>();
	newBone->Init(model);

	newBone->transform_->parent_ = static_cast<BaseTransform *>(transform_);

	newBone->SetTransform(srt);

	modelKey_.insert({ key,newBone.get() });

	modelTree_.push_back(std::move(newBone));


	return modelKey_.at(key);
}

ModelComp::ModelBone *const ModelComp::AddBone(const std::string &key, Model *const model, ModelBone *const parent, const BaseTransform &srt) {
	if (modelKey_.count(key) != 0u) return nullptr;

	auto *const newBone = parent->AddChild(model);

	newBone->SetTransform(srt);

	modelKey_.insert({ key,newBone });

	return newBone;
}
