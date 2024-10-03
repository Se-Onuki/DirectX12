#pragma once
#include "../Entity.h"

#include <unordered_map>
#include <memory>

#include "../../../Engine/DirectBase/Model/Model.h"
#include "../../Engine/Utils/Math/Transform.h"

class ModelComp : public IComponent {
public:
	struct ModelBone {
		Transform transform_;
		Model *model_;

		ModelBone *parent_ = nullptr;
		std::list<std::unique_ptr<ModelBone>> children_;

		void Init(Model *const model = nullptr);
		void SetTransform(const BaseTransform &srt);

		ModelBone *const AddChild(Model *const model);
		void AddChild(ModelBone *const child);
		void SetParent(ModelBone *const parent);

		void Update();
		void Draw(const Camera<Render::CameraType::Projecction> &vp) const;
		void Draw(const Camera<Render::CameraType::Projecction> &vp, const Material &material) const;

		bool ImGuiWidget();
	};

	//using ModelPair = std::pair<Transform, Model *>;
	//using ModelMap = std::unordered_map<std::string, ModelPair>;
	// 親のコンストラクタを使う
	using IComponent::IComponent;
	~ModelComp() override = default;

	//void SetModel(const ModelMap &model);

	/*ModelMap *const GetModel() {
		return &modelMap_;
	}

	ModelPair *const GetModel(const std::string &key) {
		auto it = modelMap_.find(key);
		if (it != modelMap_.end()) {
			return &it->second;
		}
		return nullptr;
	}*/


	void Update() override;
	void Draw(const Camera<Render::CameraType::Projecction> &vp)const override;
	void Draw(const Camera<Render::CameraType::Projecction> &vp, const Material &material) const;

	void ImGuiWidget() override;

	ModelBone *const AddBone(const std::string &key, Model *const model, const BaseTransform &srt = {});
	ModelBone *const AddBone(const std::string &key, Model *const model, ModelBone *const parent, const BaseTransform &srt);

	ModelBone *const GetBone(const std::string &key) { return modelKey_.at(key); }

	const auto &GetModelTree() const { return modelTree_; }

private:
	std::list<std::unique_ptr<ModelBone>> modelTree_;
	std::unordered_map<std::string, ModelBone *> modelKey_;

};