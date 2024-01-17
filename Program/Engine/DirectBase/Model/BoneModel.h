#pragma once
#include "Model.h"
#include <unordered_map>
#include <array>
#include "../../Header/Object/Block/BlockManager.h"

class BoneModel {
public:
	class Bone {

		// Matrix4x4 transMat_;
		Model *model_;

		Bone *parent_ = nullptr;
		std::list<std::unique_ptr<Bone>> children_;

	public:

		void Init(Model *const model = nullptr) { if (model) { model_ = model; } }

		Bone *AddChild(Model *const model);
		Bone *AddChild(std::unique_ptr<Bone> child);

		void SetParent(Bone *const parent);
		Bone *GetParent() const { return parent_; }

		const auto &GetChild() const { return children_; }

		std::list<const Bone *> GetBoneList() const;

		Model *GetModel() const { return model_; }

	};

	struct BoneTransform {

		Vector3 scale_;
		Quaternion rotate_;
		Vector3 transform_;

		Matrix4x4 transMat_;

		void CalcTransMat(const Matrix4x4 *parent = nullptr);

	};

public:

	template<size_t I>
	void CalcTransMat(std::array<BoneTransform, I> &boneTrans) const;

	void SetNumber();

	void Init();

	template<size_t I>
	void Draw(const std::array<BoneTransform, I> &boneTrans) const;

private:

	std::unique_ptr<Bone> bone_;
	std::unordered_map<const Bone *, uint32_t> boneNumberMap_;

};

template<size_t I>
inline void BoneModel::CalcTransMat(std::array<BoneTransform, I> &boneTrans) const {

	for (auto &bone : bone_->GetBoneList()) {
		BoneTransform &item = boneTrans[boneNumberMap_.at(bone)];

		item.CalcTransMat(bone->GetParent());

	}

}

template<size_t I>
inline void BoneModel::Draw(const std::array<BoneTransform, I> &boneTrans) const {
	static BlockManager *const blockManager = BlockManager::GetInstance();


	for (auto &bone : bone_->GetBoneList()) {
		BoneTransform &item = boneTrans[boneNumberMap_.at(bone)];

		if (not bone->GetModel()) { continue; }

		blockManager->AddBox(bone->GetModel(), IBlock{ .transMat_ = item.transMat_ });

	}

}
