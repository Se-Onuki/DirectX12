#pragma once
#include "Model.h"
#include <unordered_map>
#include <array>
#include "../../Header/Object/Block/BlockManager.h"

class BoneModel {
public:
	class Bone;

	class Box {
		friend Bone;
		// モデルデータ
		Model *model_;

		// 親のボーン
		Bone *parent_ = nullptr;

	public:
		Box(Model *model) :model_(model) {};

		/// @brief 親ボーンのゲッタ
		/// @return 親ボーンのアドレス
		Bone *GetParent() const { return parent_; }

		/// @brief モデルデータのゲッタ
		/// @return モデルデータアドレス
		Model *GetModel() const { return model_; }

	};

	class Bone {
		// 親のボーン
		Bone *parent_ = nullptr;
		// 子供のボーン
		std::list<std::unique_ptr<Bone>> boneChildren_;
		// 紐づいているモデル
		std::vector<std::unique_ptr<Box>> boxArray_;

	public:

		Box *AddBox(Model *const model);
		uint32_t AddBox(Box &&box);
		Bone *AddChild(std::unique_ptr<Bone> child);
		Bone *AddChild();

		void SetParent(Bone *const parent);
		Bone *GetParent() const { return parent_; }

		const auto &GetChild() const { return boneChildren_; }

		std::list<const Bone *> GetBoneList() const;

		std::list<const Box *> GetBoxList() const;

		Box *const GetBox(const uint32_t index) { return boxArray_[index].get(); }


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
		Vector3 translate_{};

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

	Bone *AddBone(const std::string &key, Bone *parent = nullptr);

	/// @brief ボーンの取得
	/// @param key 文字列キー
	/// @return ボーンのポインタ
	Bone *GetBone(const std::string &key) const;

	uint32_t GetIndex(const std::string &key) const;
	uint32_t GetIndex(const std::string &key, const uint32_t boxIndex) const;

	template<size_t I>
	void Draw(const std::array<BoneTransform, I> &boneTrans) const;

	template<size_t I>
	void Draw(const std::array<Matrix4x4, I> &boneTrans) const;

private:

	std::unique_ptr<Bone> bone_;
	std::unordered_map<const Bone *, uint32_t> boneNumberMap_;
	std::unordered_map<const Box *, uint32_t> boxNumberMap_;

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

	// 返す行列
	std::array<Matrix4x4, I> result;

	// 全て初期化
	std::fill_n(result.data(), I, Matrix4x4::Identity());

	// ボーンのトランスフォームを行列に格納
	for (const auto *const bone : bone_->GetBoneList()) {
		// ボーンに対応した添え字を返す
		uint32_t itemNumber = boneNumberMap_.at(bone);
		// 対応したTransformを取得
		SimpleTransform &item = boneTrans[itemNumber];

		// 親の情報を取得
		auto *parent = bone->GetParent();
		// 親の行列
		Matrix4x4 *parentMat = nullptr;

		// 親が存在するなら行列に保存
		if (parent) {
			parentMat = &result[boneNumberMap_.at(parent)];
		}

		// トランスフォームから行列を生成
		result[itemNumber] = item.CalcTransMat(parentMat);

	}

	// 箱のトランスフォームを行列に格納
	for (const auto *const box : bone_->GetBoxList()) {
		// 箱に対応した添え字を返す
		uint32_t itemNumber = boxNumberMap_.at(box);
		// 対応したTransformを取得
		SimpleTransform &item = boneTrans[itemNumber];

		// 親の情報を取得
		auto *parent = box->GetParent();
		// 親の行列
		Matrix4x4 *parentMat = nullptr;

		// 親が存在するなら行列に保存
		if (parent) {
			parentMat = &result[boneNumberMap_.at(parent)];
		}

		// トランスフォームから行列を生成
		result[itemNumber] = item.CalcTransMat(parentMat);

	}

	return result;
}

template<size_t I>
inline void BoneModel::Draw(const std::array<BoneTransform, I> &boneTrans) const {
	static BlockManager *const blockManager = BlockManager::GetInstance();


	for (auto &bone : bone_->GetBoneList()) {
		const BoneTransform &item = boneTrans[boneNumberMap_.at(bone)];

		/*if (not bone->GetModel()) { continue; }

		blockManager->AddBox(bone->GetModel(), IBlock{ .transMat_ = item.transMat_ });*/

	}

}

template<size_t I>
inline void BoneModel::Draw(const std::array<Matrix4x4, I> &boneTrans) const {
	static BlockManager *const blockManager = BlockManager::GetInstance();


	for (auto &box : bone_->GetBoxList()) {

		const Matrix4x4 &item = boneTrans[boxNumberMap_.at(box)];

		const auto *const model = box->GetModel();
		if (not model) { continue; }

		blockManager->AddBox(model, IBlock{ .transMat_ = item });

	}

}
