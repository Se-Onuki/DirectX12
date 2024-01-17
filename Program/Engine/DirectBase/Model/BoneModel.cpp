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

		result.merge(child->GetBoneList());
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

void BoneModel::BoneTransform::CalcTransMat(const Matrix4x4 *parent) {
	transMat_ = SoLib::Math::Affine(scale_, rotate_, transform_);

	// 親が指定されていた場合は計算
	if (parent) {
		transMat_ *= *parent;
	}

}
