#include "BoneModel.h"

BoneModel::Box *BoneModel::Bone::AddBox(Model *const model) {
	// モデルを保存したBoxを追加して、その添え字を返す
	uint32_t index = AddBox(Box{ model });

	// 添え字を使ってデータを取得する。
	return boxArray_[index].get();
}
uint32_t BoneModel::Bone::AddBox(Box &&box) {
	// ボックスを作成
	auto boxItem = std::make_unique<Box>(std::move(box));
	// 親を指定
	boxItem->parent_ = this;

	// 配列に追加
	boxArray_.push_back(std::move(boxItem));
	// 添え字を返す
	return static_cast<uint32_t>(boxArray_.size() - 1u);
}


BoneModel::Bone *BoneModel::Bone::AddChild(std::unique_ptr<Bone> child) {
	child->SetParent(this);
	this->boneChildren_.push_back(std::move(child));

	return boneChildren_.back().get();
}

BoneModel::Bone *BoneModel::Bone::AddChild()
{
	return AddChild(std::make_unique<Bone>());
}

void BoneModel::Bone::SetParent(Bone *const parent) {

	parent_ = parent;

}

std::list<const BoneModel::Bone *> BoneModel::Bone::GetBoneList() const {
	std::list<const Bone *> result{};

	result.push_back(this);

	for (const auto &child : boneChildren_) {
		auto childList = child->GetBoneList();
		result.splice(result.end(), std::move(childList));
	}

	return result;
}

std::list<const BoneModel::Box *> BoneModel::Bone::GetBoxList() const {
	std::list<const Box *> result{};

	// boxの情報をコピー
	for (const auto &boxItem : boxArray_) {
		result.push_back(boxItem.get());
	}

	// 子供のボーンをfor文で回す
	for (const auto &child : boneChildren_) {
		// ボーンのリストを取得(再帰関数)
		auto boxList = child->GetBoxList();

		// 自分自身のリストの末尾に子供の配列を結合
		result.splice(result.end(), std::move(boxList));
	}

	return result;
}

void BoneModel::SetNumber() {

	boneNumberMap_.clear();
	boxNumberMap_.clear();
	uint32_t boneNumber = 0u;

	for (auto &bone : bone_->GetBoneList()) {

		boneNumberMap_[bone] = boneNumber++;

	}

	for (auto &box : bone_->GetBoxList()) {

		boxNumberMap_[box] = boneNumber++;

	}

}

void BoneModel::Init() {
	bone_ = std::make_unique<Bone>();
}

BoneModel::Bone *BoneModel::AddBone(const std::string &key, Model *model, Bone *parent) {

	Bone *newBone = AddBone(key, parent);
	newBone->AddBox(model);

	return newBone;
}

BoneModel::Bone *BoneModel::AddBone(const std::string &key, Bone *parent)
{
	// 親ボーン
	Bone *pParent = bone_.get();
	// 引数で親が指定されていた場合はそれに変える
	if (parent) {
		pParent = parent;
	}

	// ボーンの追加
	Bone *newBone = pParent->AddChild();

	// ボーンを文字列に登録する
	boneKeyMap_[key] = newBone;

	// ボーンのアドレスを返す
	return newBone;
}

BoneModel::Bone *BoneModel::GetBone(const std::string &key) const {

	// マップから検索
	auto modelItr = boneKeyMap_.find(key);

	// もし存在してなかったら何も返さない
	if (modelItr == boneKeyMap_.end()) {
		return nullptr;
	}
	// ボーンのポインタを返す
	return modelItr->second;
}

uint32_t BoneModel::GetIndex(const std::string &key) const {
	auto bone = GetBone(key);
	// ボーンが存在するなら
	if (bone) {
		// 対応したindexを返す
		return boneNumberMap_.at(bone);
	}

#ifdef _DEBUG
	assert(0 && "文字列に対応するボーンが保存されていません。");
#endif // _DEBUG

	// 存在しないなら
	return (std::numeric_limits<uint32_t>::max)();
}

uint32_t BoneModel::GetIndex(const std::string &key, const uint32_t boxIndex) const {
	auto bone = GetBone(key);
	// ボーンが存在するなら
	if (bone) {
		// 対応したindexを返す
		return boxNumberMap_.at(bone->GetBox(boxIndex));
	}

#ifdef _DEBUG
	assert(0 && "文字列に対応するボーンが保存されていません。");
#endif // _DEBUG

	// 存在しないなら
	return (std::numeric_limits<uint32_t>::max)();
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
