#pragma once
#include "../Object.h"

#include <unordered_map>
#include "../../Model/Model.h"

class ModelComp : public IComponent {

	std::unordered_map<std::string, std::pair<Transform, Model *>> modelMap_;
public:
	ModelComp() = default;
	~ModelComp() override = default;

	void SetModel(const std::unordered_map<std::string, std::pair<Transform, Model *>> &model);

	void Update() override;
	void Draw(const ViewProjection &vp)const override;

private:
	/// @brief モデルの親子関係を紐づける
	void SetTransformParent();

};