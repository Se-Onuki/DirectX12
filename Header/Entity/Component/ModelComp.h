#pragma once
#include "../Entity.h"

#include <unordered_map>
#include <memory>

#include "../../../Engine/DirectBase/Model/Model.h"
#include "../../../Utils/Math/Transform.h"

class ModelComp : public IComponent {
public:
	struct ModelBone {
		Transform transform_;
		Model *model_;

		ModelBone *parent_ = nullptr;
		std::list<std::unique_ptr<ModelBone>> children_;

		void Init(Model *const model = nullptr);
		void SetTransform(const Transform &srt);

		ModelBone *const AddChild(Model *const model);
		void AddChild(ModelBone *const child);
		void SetParent(ModelBone *const parent);

		void Update();
		void Draw(const Camera<Render::CameraType::Projecction> &vp) const;
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

	ModelBone *const AddBone(const std::string &key, Model *const model, const Transform &srt = {});
	ModelBone *const AddBone(const std::string &key, Model *const model, ModelBone *const parent, const Transform &srt);

	ModelBone *const GetBone(const std::string &key) { return modelKey_.at(key); }


private:
	std::list<std::unique_ptr<ModelBone>> modelTree_;
	std::unordered_map<std::string, ModelBone *> modelKey_;

};