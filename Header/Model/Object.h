#pragma once
#include <string>
#include <memory>
#include "Model.h"
#include "../Math/Transform.h"


class Object {
public:
	Object(const std::string &filePath) {
		model_.reset(Model::LoadObjFile("", filePath));
		transform_.InitResource();
	}
	~Object() = default;

	std::unique_ptr<Model> model_ = nullptr;
	Transform transform_{};
	void ImGuiWidget() {
		transform_.ImGuiWidget();
		model_->ImGuiWidget();
	}
	void Draw(const ViewProjection &viewProjection)const {
		model_->Draw(transform_, viewProjection);
	}
};
