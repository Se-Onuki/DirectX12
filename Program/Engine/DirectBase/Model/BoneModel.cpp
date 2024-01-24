#include "BoneModel.h"

BoneModel::Bone *BoneModel::Bone::AddChild(Model *const model) {
	auto bone = std::make_unique<Bone>();
	bone->Init(model);

	return AddChild(std::move(bone));
}

BoneModel::Bone *BoneModel::Bone::AddChild(std::unique_ptr<Bone> child) {
	child->SetParent(this);
	this->children_.push_back(std::move(child));

	return children_.back().get();
}

void BoneModel::Bone::SetParent(Bone *const parent) {

	parent_ = parent;

}

std::list<const BoneModel::Bone *> BoneModel::Bone::GetBoneList() const {
	std::list<const Bone *> result{};

	result.push_back(this);

	for (const auto &child : children_) {
		auto childList = child->GetBoneList();
		result.splice(result.end(), std::move(childList));
	}

	return result;
}

void BoneModel::SetNumber() {

	boneNumberMap_.clear();
	uint32_t boneNumber = 0u;

	for (auto &bone : bone_->GetBoneList()) {

		boneNumberMap_[bone] = boneNumber++;

	}

}

void BoneModel::Init() {
	bone_ = std::make_unique<Bone>();
}

BoneModel::Bone *BoneModel::AddBone(const std::string &key, Model *model, Bone *parent) {
	Bone *pParent = bone_.get();
	if (parent) {
		pParent = parent;
	}

	Bone *newBone = pParent->AddChild(model);

	modelKey_[key] = newBone;

	return newBone;
}

bool BoneModel::BoneTransform::ImGuiWidget(const char *const label) {
	bool isChanged = false;
	if (ImGui::TreeNode(label)) {
		isChanged |= SoLib::ImGuiWidget("Scale", &scale_);
		isChanged |= ImGui::DragFloat4("Rotate", &rotate_.x);
		isChanged |= SoLib::ImGuiWidget("Position", &translate_);
		ImGui::TreePop();
	}

	return isChanged;
}

void BoneModel::BoneTransform::CalcTransMat(const Matrix4x4 *parent) {
	transMat_ = SoLib::Math::Affine(scale_, rotate_, translate_);

	// 親が指定されていた場合は計算
	if (parent) {
		transMat_ *= *parent;
	}

}

bool BoneModel::SimpleTransform::ImGuiWidget(const char *const label) {
	bool isChanged = false;
	if (ImGui::TreeNode(label)) {
		isChanged |= SoLib::ImGuiWidget("Scale", &scale_);
		isChanged |= ImGui::DragFloat4("Rotate", &rotate_.x);
		isChanged |= SoLib::ImGuiWidget("Position", &translate_);
		ImGui::TreePop();
	}

	return isChanged;
}

Matrix4x4 BoneModel::SimpleTransform::CalcTransMat(const Matrix4x4 *parent) {
	Matrix4x4 transMat = SoLib::Math::Affine(scale_, rotate_, translate_);

	// 親が指定されていた場合は計算
	if (parent) {
		transMat *= *parent;
	}

	return transMat;
}
