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

		Vector3 scale_ = Vector3::one;
		Quaternion rotate_ = Quaternion::Identity;
		Vector3 translate_;

		bool ImGuiWidget(const char *const label);

		Matrix4x4 transMat_;

		void CalcTransMat(const Matrix4x4 *parent = nullptr);

	};

	struct SimpleTransform {
		Vector3 scale_ = Vector3::one;
		Quaternion rotate_ = Quaternion::Identity;
		Vector3 translate_;

		bool ImGuiWidget(const char *const label);

		Matrix4x4 CalcTransMat(const Matrix4x4 *parent = nullptr);

	};

public:

	template<size_t I>
	void CalcTransMat(std::array<BoneTransform, I> &boneTrans) const;

	template<size_t I>
	std::array<Matrix4x4, I> CalcTransMat(std::array<SimpleTransform, I> &boneTrans) const;

	void SetNumber();

	void Init();

	/// @brief ボーンの追加
	/// @param key 紐づける文字列
	/// @param model モデル
	/// @param parent 親ボーン
	/// @return 追加されたボーン
	Bone *AddBone(const std::string &key, Model *model, Bone *parent = nullptr);

	/// @brief ボーンの取得
	/// @param key 文字列キー
	/// @return ボーンのポインタ
	Bone *GetBone(const std::string &key) const;

	uint32_t GetIndex(const std::string &key) const;

	template<size_t I>
	void Draw(const std::array<BoneTransform, I> &boneTrans) const;

	template<size_t I>
	void Draw(const std::array<Matrix4x4, I> &boneTrans) const;

private:

	std::unique_ptr<Bone> bone_;
	std::unordered_map<const Bone *, uint32_t> boneNumberMap_;

	std::unordered_map<std::string, Bone *> boneKeyMap_;

};

template<size_t I>
inline void BoneModel::CalcTransMat(std::array<BoneTransform, I> &boneTrans) const {

	for (auto &bone : bone_->GetBoneList()) {
		BoneTransform &item = boneTrans[boneNumberMap_.at(bone)];

		auto *parent = bone->GetParent();
		Matrix4x4 *parentMat = nullptr;
		if (parent) {
			parentMat = &boneTrans[boneNumberMap_.at(parent)].transMat_;
		}

		item.CalcTransMat(parentMat);

	}

}
template<size_t I>
inline std::array<Matrix4x4, I> BoneModel::CalcTransMat(std::array<SimpleTransform, I> &boneTrans) const {

	std::array<Matrix4x4, I> result;
	//result.
	std::fill_n(result.data(), I, Matrix4x4::Identity());

	for (const auto *const bone : bone_->GetBoneList()) {
		uint32_t itemNumber = boneNumberMap_.at(bone);
		SimpleTransform &item = boneTrans[itemNumber];

		auto *parent = bone->GetParent();
		Matrix4x4 *parentMat = nullptr;
		if (parent) {
			parentMat = &result[boneNumberMap_.at(parent)];
		}

		result[itemNumber] = item.CalcTransMat(parentMat);

	}

	return result;
}

template<size_t I>
inline void BoneModel::Draw(const std::array<BoneTransform, I> &boneTrans) const {
	static BlockManager *const blockManager = BlockManager::GetInstance();


	for (auto &bone : bone_->GetBoneList()) {
		const BoneTransform &item = boneTrans[boneNumberMap_.at(bone)];

		if (not bone->GetModel()) { continue; }

		blockManager->AddBox(bone->GetModel(), IBlock{ .transMat_ = item.transMat_ });

	}

}

template<size_t I>
inline void BoneModel::Draw(const std::array<Matrix4x4, I> &boneTrans) const {
	static BlockManager *const blockManager = BlockManager::GetInstance();


	for (auto &bone : bone_->GetBoneList()) {
		const Matrix4x4 &item = boneTrans[boneNumberMap_.at(bone)];

		if (not bone->GetModel()) { continue; }

		blockManager->AddBox(bone->GetModel(), IBlock{ .transMat_ = item });

	}

}
