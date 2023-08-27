#pragma once
#include "../Object.h"

#include <unordered_map>
#include <memory>
#include "../../Model/Model.h"
#include "../../Math/Transform.h"

class ModelComp : public IComponent {
public:
	struct ModelTree {
		Transform transform_;
		Model *model_;

		ModelTree *parent_ = nullptr;
		std::list<std::unique_ptr<ModelTree>> children_;

		void Init();
		void AddChild(Model *const model);
		void SetParent(ModelTree *parent);
	};

	using ModelPair = std::pair<Transform, Model *>;
	using ModelMap = std::unordered_map<std::string, ModelPair>;
	// 親のコンストラクタを使う
	using IComponent::IComponent;
	~ModelComp() override = default;

	void SetModel(const ModelMap &model);

	ModelMap *const GetModel() {
		return &modelMap_;
	}

	ModelPair *const GetModel(const std::string &key) {
		auto it = modelMap_.find(key);
		if (it != modelMap_.end()) {
			return &it->second;
		}
		return nullptr;
	}


	void Update() override;
	void Draw(const ViewProjection &vp)const override;

private:
	/// @brief モデルの親子関係を紐づける
	void SetTransformParent();

	ModelMap modelMap_;

	std::list<ModelTree> modelTree_;
	std::unordered_map<std::string, ModelTree *> modelKey_;

};