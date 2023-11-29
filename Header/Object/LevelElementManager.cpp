#include "LevelElementManager.h"

#include <numeric>

#include "../../Engine/DirectBase/File/GlobalVariables.h"
#include "../../Engine/DirectBase/Model/ModelManager.h"
#include "../../StarItemComp.h"
#include "../../Utils/SoLib/SoLib_Easing.h"
#include "../../Utils/SoLib/SoLib_ImGui.h"
#include "../Entity/Component/PlayerComp.h"
#include "../Entity/Component/Rigidbody.h"
#include "../Object/Block/BlockManager.h"
#include "imgui.h"
#include "../Entity/Component/PlayerState/IdleState.h"
#include "Fade.h"
#include "TutorialManager.h"
#include "InGameUIManager/InGameUIManager.h"

nlohmann::json LevelElementManager::levelData_;

void LevelElementManager::StaticInit()
{
	std::ifstream file("resources/LevelData.json");
	file >> levelData_;
}

void LevelElementManager::LoadData(const uint32_t levelID)
{
	nowLevel_ = levelID;
	monoLevelData_ = GetLevelParameters(levelData_, levelID);
	SetData();

	pPlayer_->transform_.translate = startPos_->translate;
}

void LevelElementManager::SetData()
{
	// 足場データの破棄
	blockCollider_.clear();
	// カメラの始点と終点の設定
	lineStart_->translate = monoLevelData_["CameraStart"];
	lineEnd_->translate = monoLevelData_["CameraEnd"];

	lineStart_->UpdateMatrix();
	lineEnd_->UpdateMatrix();

	stageLine_.origin = lineStart_->translate;
	stageLine_.SetEnd(lineEnd_->translate);

	startPos_->translate = monoLevelData_["StartPos"];
	pPlayer_->GetComponent<PlayerComp>()->vRotateHeight_.GetItem() = monoLevelData_["SpinHight"];

	// 足場のJsonの取得
	const nlohmann::json &platforms = monoLevelData_["Platform"];
	// 足場のID
	uint32_t platformID = 0;
	// blockCollider_[platformID];
	//  足場の追加
	for (const auto &pratItr : platforms) {
		// 足場データの参照を取得
		Platform &platform = blockCollider_[platformID];
		// 回転の中心の設定
		platform.center_.translate = pratItr["Centor"];
		// 回転方向
		platform.rotateAxis_ = pratItr["Axis"];
		platform.axisBar_->scale.x = pratItr["AxisBarLength"];

		if (platform.rotateAxis_ == Vector3::front) {
			platform.axisBar_->rotate.y = Angle::PI / 2.f;
		}
		else {
			platform.axisBar_->rotate.y = 0.f;
		}

		// ゴールがあったら
		if (pratItr.find("Goal") != pratItr.end()) {

			platform.CreateGoal();
			platform.GetGoal()->transform_.translate = pratItr["Goal"]["Pos"];
			platform.GetGoal()->transform_.rotate = pratItr["Goal"]["Rotate"];
		}

		// ブロックの追加
		for (const auto &box : pratItr["Box"]) {
			Vector3 pos;
			pos = box["Centor"];
			Vector3 scale;
			scale = box["Radius"];
			platform.AddBox(AABB::Create(pos, scale), static_cast<GroundType>(box["GroundType"].get<int32_t>()));
		}
		// アイテムの追加
		for (const auto &item : pratItr["StarItem"]) {
			BaseTransform srt;
			srt.translate = item["Pos"];
			srt.rotate = item["Rotate"];

			platform.AddItem(srt);
		}
		platform.center_.CalcMatrix();
		platform.axisBar_->UpdateMatrix();

		platform.CalcCollision();
		platformID++;
	}

	this->SetTransferData();

	this->vMaxRotateCount_ = static_cast<int32_t>(GetStarItemList().size());
}

void LevelElementManager::SaveData()
{
	monoLevelData_["CameraStart"] = lineStart_->translate;
	monoLevelData_["CameraEnd"] = lineEnd_->translate;

	monoLevelData_["StartPos"] = startPos_->translate;

	monoLevelData_["SpinHight"] = pPlayer_->GetComponent<PlayerComp>()->vRotateHeight_.GetItem();

	nlohmann::json platforms;

	for (auto &[key, pratItr] : blockCollider_) {

		nlohmann::json platData;

		platData["Centor"] = pratItr.center_.translate;
		platData["Axis"] = pratItr.rotateAxis_;
		platData["AxisBarLength"] = pratItr.axisBar_->scale.x;

		auto *const goal = pratItr.GetGoal();
		if (goal) {
			platData["Goal"]["Pos"] = goal->transform_.translate;
			platData["Goal"]["Rotate"] = goal->transform_.rotate;
		}
		// ボックスの追加
		nlohmann::json boxList;
		for (const auto &box : pratItr.GetBoxList()) {
			nlohmann::json boxData;

			boxData["Centor"] = box.transform_->translate;
			boxData["Radius"] = box.referenceBox_.GetRadius();
			boxData["GroundType"] = static_cast<int32_t>(box.groundType_);

			boxList.push_back(boxData);
		}
		// アイテムの追加
		nlohmann::json itemList;
		for (const auto &item : pratItr.GetStarItem()) {
			nlohmann::json itemData;

			itemData["Pos"] = item->transform_.translate;
			itemData["Rotate"] = item->transform_.rotate;

			itemList.push_back(itemData);
		}
		platData["Box"] = boxList;
		platData["StarItem"] = itemList;

		platforms.push_back(platData);
	}

	monoLevelData_["Platform"] = platforms;

#pragma region SaveData

	levelData_["Level"][nowLevel_] = monoLevelData_;

	std::ofstream file("resources/LevelData.json");

	file << std::setw(4) << levelData_;
#pragma endregion
}

bool LevelElementManager::AnyPlatformRotating() const
{
	// どれか一つでも回転していないか
	bool anyTrue =
		std::any_of(this->blockCollider_.begin(), this->blockCollider_.end(),
			[](const auto &pair)
			{
				return pair.second.GetTimer().IsActive() == true;
			});
	return anyTrue;
}

bool LevelElementManager::Undo() {
	// どれか一つでも回転していないか
	bool anyTrue = AnyPlatformRotating();
	// 回転していたら終了
	if (anyTrue) {
		return false;
	}

	// もしログがあったら実行
	if (stateLog_.size()) {
		// 操作ログを末尾から1つ吐き出す
		const StateLog log = stateLog_.back();
		stateLog_.pop_back();

		// 戻すデータを登録
		undoLog_ = log;

		// タイマーを開始
		undoTimer_.Start(vLerpTime_);
		return true;
	}
	return false;
}

void LevelElementManager::UndoUpdate(const float deltaTime)
{
	undoTimer_.Update(deltaTime);

	// タイマーが動作していて、回転操作が発生していなかったら
	if (undoTimer_.IsActive() && AnyPlatformRotating() == false) {
		for (const auto &rotPair : undoLog_.angleList_) {
			this->GetPlatform(rotPair.first)->SetRotate(rotPair.second);
		}
	}
	// 終了したら
	if (undoTimer_.IsFinish() && undoTimer_.IsActive()) {

		pPlayer_->GetComponent<PlayerComp>()->ChangeState<PlayerIdleState>();
		pPlayer_->GetComponent<PlayerAnimComp>()->GetAnimManager()->SetNextAnimation(PlayerBehavior::kIdle, true);
		if (undoLog_.item_) {
			pPlayer_->transform_.translate = undoLog_.item_->transform_.GetGrobalPos();
			undoLog_.item_->GetComponent<StarItemComp>()->Reset();
		}
		else {
			pPlayer_->transform_.translate = startPos_->translate;
		}
		remainRotateCount_ = undoLog_.remainRotCount_;
		pInGameUI_->SetStar(remainRotateCount_);

		Fade::GetInstance()->Start({ 0.0f, 0.0f }, { 0.0f,0.0f, 0.0f, 0.0f }, 0.25f);

		// もしリストが空だったら戻す
		if (stateLog_.size() == 0u) {
			AddUndoLog(nullptr);
		}
		// undoLog_ = StateLog{ .item_ = nullptr, .remainRotCount_ =
		// remainRotateCount_, .angleList_ = std::move(angleList);
	}
}

void LevelElementManager::ImGuiWidget()
{
	ImGui::Checkbox("DebugViewer", &debugDrawer_);
	if (debugDrawer_) {
		pPlayer_->GetComponent<PlayerComp>()->SetIsActiveGravity(false);
		Vector3 velocity = pPlayer_->GetComponent<Rigidbody>()->GetVelocity();
		velocity.y = 0.f;
		pPlayer_->GetComponent<Rigidbody>()->SetVelocity(velocity);
		// pPlayer_->GetComponent<Rigidbody>()->
	}
	else {
		pPlayer_->GetComponent<PlayerComp>()->SetIsActiveGravity(true);
	}

	ImGui::DragFloat3("PlayerStartPos", &startPos_->translate.x, 1.f);
	startPos_->UpdateMatrix();
	ImGui::DragFloat("SpinHight", &pPlayer_->GetComponent<PlayerComp>()->vRotateHeight_.GetItem());

	// カメラの位置は編集されたか
	bool isCameraEditedBy = false;
	isCameraEditedBy |=
		ImGui::DragFloat3("CameraLineStart", &lineStart_->translate.x, 1.f);
	isCameraEditedBy |=
		ImGui::DragFloat3("CameraLineEnd", &lineEnd_->translate.x, 1.f);
	if (lineStart_->translate == lineEnd_->translate) {
		lineEnd_->translate.z++;
	}

	// もし編集されたら
	if (isCameraEditedBy) {
		lineStart_->UpdateMatrix();
		lineEnd_->UpdateMatrix();

		stageLine_.origin = lineStart_->translate;
		stageLine_.SetEnd(lineEnd_->translate);
	}

	std::vector<int32_t> items;
	for (const auto &pair : blockCollider_) {
		items.push_back(pair.first);
	}
	if (ImGui::Button("AddPlatform")) {
		blockCollider_[static_cast<uint32_t>(blockCollider_.size())];
		platformItr_ = --blockCollider_.end();
	}
	if (platformItr_ == blockCollider_.end()) {
		platformItr_ = blockCollider_.begin();
	}
	if (platformItr_ != blockCollider_.end()) {
		if (ImGui::BeginCombo("PlatformList",
			std::to_string(platformItr_->first).c_str())) {
			for (PlatformMap::iterator it = blockCollider_.begin();
				it != blockCollider_.end(); it++) {
				if (ImGui::Selectable(std::to_string(it->first).c_str())) {
					platformItr_ = it;
					break;
				}
			}
			ImGui::EndCombo();
		}

		if (platformItr_->second.ImGuiWidget()) {

			if (platformItr_->second.isDelete_) {
				blockCollider_.erase(platformItr_);
				platformItr_ = blockCollider_.begin();
			}
			this->SetTransferData();
		}
	}

	if (ImGui::Button("Save")) {
		SaveData();
	}
}

void LevelElementManager::AddUndoLog(Entity *const starItem)
{
	std::list<std::pair<uint32_t, float>> angleList;

	for (const auto &platform : blockCollider_) {
		std::pair<uint32_t, float> keyAngle;
		keyAngle.first = platform.first;
		keyAngle.second =
			std::accumulate(platform.second.center_.rotate.begin(),
				platform.second.center_.rotate.end(), 0.f);

		angleList.push_back(keyAngle);
	}

	stateLog_.push_back(StateLog{ .item_ = starItem,
								 .remainRotCount_ = remainRotateCount_,
								 .angleList_ = std::move(angleList) });
}

void LevelElementManager::Init()
{
	const auto *const modelManager = ModelManager::GetInstance();
	groundModels_[static_cast<uint32_t>(GroundType::kDirt)] =
		modelManager->GetModel("DirtModel");
	groundModels_[static_cast<uint32_t>(GroundType::kGrass)] =
		modelManager->GetModel("GrassModel");

	if (lineStart_->translate == lineEnd_->translate) {
		lineEnd_->translate.z = lineStart_->translate.z + 1.f;
	}

	lineStart_->scale = Vector3::one * 0.5f;
	lineEnd_->scale = Vector3::one * 0.5f;

	lineStart_->parent_ = nullptr;
	lineEnd_->parent_ = nullptr;

	lineStart_->UpdateMatrix();
	lineEnd_->UpdateMatrix();

	stageLine_.origin = lineStart_->translate;
	stageLine_.SetEnd(lineEnd_->translate);

	BlockManager::GetInstance()->clear();

	blockCollider_.clear();

	// undo処理の破棄
	stateLog_.clear();
	undoLog_ = StateLog{
		.item_ = nullptr,
		.remainRotCount_ = -1,
	};
	undoTimer_.Clear();

	remainRotateCount_ = 0;

	platformItr_ = blockCollider_.begin();
}

void LevelElementManager::Update([[maybe_unused]] float deltaTime)
{
	UndoUpdate(deltaTime);
	for (auto &[key, platform] : blockCollider_) {
		platform.Update(deltaTime);
	}

#ifdef _DEBUG

	ImGui::Text("RotateCount : %i / %i", remainRotateCount_,
		vMaxRotateCount_.GetItem());

#endif // _DEBUG
}

void LevelElementManager::Draw([[maybe_unused]] const Camera3D &camera) const
{
	// static const ModelManager *const modelManager =
	// ModelManager::GetInstance();

	for (const auto &[key, platform] : blockCollider_) {
		// const Model *const model = groundModels_[pratItr.]
		platform.Draw(camera);
	}

	for (const auto &goal : GetGoalList()) {
		goal->Draw(camera);
	}

	for (const auto &item : this->GetStarItemList()) {
		item->Draw(camera);
	}


#ifdef _DEBUG

	if (debugDrawer_) {
		this->DebugDraw(camera);
	}

#endif // _DEBUG
}
void LevelElementManager::DebugDraw(
	[[maybe_unused]] const Camera3D &camera) const
{
	const auto *const whiteBox = ModelManager::GetInstance()->GetModel("Box");
	const auto *const sphere = ModelManager::GetInstance()->GetModel("RedBox");

	whiteBox->Draw(startPos_, camera);

	sphere->Draw(lineStart_, camera);
	sphere->Draw(lineEnd_, camera);
}
//
// void LevelElementManager::Draw(const Camera3D &camera) {
//	/*static const ModelManager *const modelManager =
// ModelManager::GetInstance(); 	const Model *const model =
// modelManager->GetModel("Box");
//
//	for (const auto &[key, pratItr] : blockCollider_) {
//		pratItr.Draw(model, camera);
//	}*/
//}

void LevelElementManager::CalcCollision(const uint32_t key)
{
	blockCollider_[key].CalcCollision();
}

void LevelElementManager::CalcCollision()
{
	for (auto &[key, platform] : blockCollider_) {
		platform.CalcCollision();
	}
}

void LevelElementManager::AddBlock(const uint32_t key, const AABB &box, GroundType groundType)
{
	blockCollider_[key].AddBox(box, groundType);
}

void LevelElementManager::AddItem(const uint32_t key,
	const BaseTransform &srt)
{
	blockCollider_[key].AddItem(srt);
}

LevelElementManager::Platform *const LevelElementManager::GetPlatform(
	int32_t index)
{
	if (index < 0) {
		return nullptr;
	}

	PlatformMap::iterator itPlatform = blockCollider_.find(index);
	if (itPlatform == blockCollider_.end()) {
		return nullptr;
	}
	return &itPlatform->second;
}

std::list<Entity *> LevelElementManager::GetStarItemList() const {
	std::list<Entity *> starItemPtrList;
	for (const auto &it : blockCollider_) {
		const auto &starItemList = it.second.GetStarItem();
		for (const auto &star : starItemList) {
			starItemPtrList.push_back(star.get());
		}
	}
	return starItemPtrList;
}

std::list<Entity *> LevelElementManager::GetGoalList() const {
	std::list<Entity *> goalList;
	for (const auto &it : blockCollider_) {
		Entity *const goal = it.second.GetGoal();
		if (goal) {
			goalList.push_back(goal);
		}
	}
	return goalList;
}

nlohmann::json LevelElementManager::GetLevelParameters(
	const nlohmann::json &jsonData, int32_t levelIndex)
{
	// レベルが存在するか確認
	if (jsonData.find("Level") == jsonData.end() ||
		!jsonData["Level"].is_array()) {
		return nlohmann::json();
	}

	// 指定された添字が有効か確認
	if (levelIndex < 0 || levelIndex >= jsonData["Level"].size()) {
		return nlohmann::json();
	}

	// 指定された添字のレベルのパラメータを取得
	nlohmann::json levelData = jsonData["Level"][levelIndex];
	return levelData;
}

void LevelElementManager::AddStarCount(const int32_t count) {
	remainRotateCount_ = std::clamp(remainRotateCount_ + count, 0, vMaxRotateCount_.GetItem());

	pInGameUI_->AddStar(count);
}

void LevelElementManager::SetTransferData() const
{
	BlockManager::GetInstance()->clear();
	uint32_t distance{};
	for (const auto &platform : this->blockCollider_) {

		for (const auto &box : platform.second.GetBoxList()) {
			box.CreateBox(LevelElementManager::GetInstance()->platformColor_[distance]);
		}
		distance++;
	}
}

LevelElementManager::Platform::Platform() : lerpTime_(LevelElementManager::GetInstance()->vLerpTime_)
{
	startRot_ = {};
	targetRot_ = {};

	boxItr_ = boxList_.begin();
	starItr_ = starItem_.begin();

	axisBar_->scale = Vector3{ 10.f, 0.5f, 0.5f };

	axisBar_->SetParent(center_);

	if (rotateAxis_ == Vector3::front) {
		axisBar_->rotate.y = Angle::PI / 2.f;
	}
	else {
		axisBar_->rotate.y = 0.f;
	}
}

LevelElementManager::Box &LevelElementManager::Platform::AddBox(const AABB &aabb, GroundType groundType)
{
	boxList_.emplace_back(aabb, this);
	auto &box = boxList_.back();
	box.transform_->parent_ = &center_;

	box.groundType_ = groundType;

	return box;
}

void LevelElementManager::Platform::AddItem(const BaseTransform &srt)
{
	starItem_.emplace_back(std::make_unique<Entity>());
	auto *const entity = starItem_.back().get();
	entity->Init();

	// auto *const itemComp =
	entity->AddComponent<StarItemComp>();

	entity->transform_ = srt;
	entity->transform_.parent_ = &this->center_;
}

void LevelElementManager::Platform::CalcCollision()
{
	center_.CalcMatrix();
	const auto &affineMat = center_.matWorld_;
	collisionBox_.clear();

	static auto *const blockManager = BlockManager::GetInstance();

	for (auto &box : boxList_) {
		AABB newBox = box.referenceBox_;
		newBox.min *= affineMat;
		newBox.max *= affineMat;
		collisionBox_.push_back(newBox.Swaping());

		box.transform_->UpdateMatrix();
	}
}

void LevelElementManager::Platform::Update(float deltaTime)
{
	timer_.Update(deltaTime);
	const Entity *const pPlayer = LevelElementManager::GetInstance()->GetPlayer();
	const auto *const playerComp = pPlayer->GetComponent<PlayerComp>();

	axisBar_->rotate.x = Angle::Mod(axisBar_->rotate.x + 2.5f * deltaTime);

	axisBar_->UpdateMatrix();

	if (goal_) {

		goal_->Update(deltaTime);

		if ((goal_->transform_.GetGrobalPos() - pPlayer->transform_.GetGrobalPos()).Length() < 2.f && (Vector3::up * goal_->transform_.matWorld_.GetRotate()) * Vector3::up > 0.3f && LevelElementManager::GetInstance()->GetStarProgress() >= 1.f && pPlayer->GetComponent<PlayerComp>()->GetIsLanding()) {
			goal_->GetComponent<GoalAnimComp>()->PlayGoalAnim();
		}
	}

	if (timer_.IsActive()) {
		center_.rotate = Angle::Lerp(startRot_, targetRot_, SoLib::easeInOutQuad(timer_.GetProgress()));
		center_.UpdateMatrix();
		for (auto &box : boxList_) {
			box.transform_->UpdateMatrix();
		}
	}
	if (timer_.IsActive() && timer_.IsFinish()) {
		startRot_ = targetRot_;
		this->CalcCollision();
	}
	for (auto &item : starItem_) {
		item->Update(deltaTime);
		auto *const itemComp = item->GetComponent<StarItemComp>();
		// 取得されていない && 接触時に取得判定を行う
		if (not itemComp->GetIsCollected() && Collision::IsHit(playerComp->GetCollider(), itemComp->GetCollider())) {
			// 取得判定を行う
			itemComp->CollectItem();
		}
	}
}

bool LevelElementManager::Platform::AddRotate(const float targetRot)
{
	if (timer_.IsFinish()) {
		TutorialManager::GetInstance()->SetProgress(TutorialManager::TutorialProgress::kDownZ);
		int32_t rotatePow{};
		if (targetRot) {
			rotatePow = static_cast<int32_t>(targetRot / std::abs(targetRot));
		}

		LevelElementManager::GetInstance()->pPlayer_->GetComponent<PlayerComp>()->rotateCount_ += rotatePow;

		targetRot_ = Angle::Mod(rotateAxis_ * targetRot + startRot_);
		timer_.Start(lerpTime_);
		return true;
	}
	return false;
}

void LevelElementManager::Platform::SetRotate(const float targetRot)
{
	if (timer_.IsFinish()) {
		targetRot_ = Angle::Mod(rotateAxis_ * targetRot);
		timer_.Start(lerpTime_);
	}
}

void LevelElementManager::Platform::Draw(const Camera3D &camera) const
{
	const auto *const box = ModelManager::GetInstance()->GetModel("Box");

	box->Draw(axisBar_, camera);
}

bool LevelElementManager::Platform::ImGuiWidget()
{
	bool isEdited = false;
	ImGui::NewLine();

	ImGui::BulletText("PlatformEditor");
	ImGui::SameLine();
	if (ImGui::Button("Delete#Platform")) {
		isDelete_ = true;
		isEdited |= true;
	}

	int32_t e;
	static std::array<Vector3, 2u> axisList = { Vector3::right, Vector3::front };
	for (std::uint32_t i = 0; i < axisList.size(); ++i) {
		if (axisList[i] == rotateAxis_) {
			e = i; // 一致した場合、インデックスを返す
		}
	};
	isEdited |= ImGui::RadioButton("RotateX", &e, 0);
	ImGui::SameLine();
	isEdited |= ImGui::RadioButton("RotateZ", &e, 1);
	rotateAxis_ = axisList[e];
	if (isEdited) {
		if (rotateAxis_ == Vector3::front) {
			axisBar_->rotate.y = Angle::PI / 2.f;
		}
		else {
			axisBar_->rotate.y = 0.f;
		}
	}

	static bool isSeparate = false;
	ImGui::Checkbox("Separate", &isSeparate);
	ImGui::SameLine();
	Vector3 preCentor = center_.translate;
	isEdited |= ImGui::DragFloat3("CentorPos", &center_.translate.x);
	if (isSeparate) {
		Vector3 centorDeff = center_.translate - preCentor * Matrix4x4::EulerRotate(-center_.rotate);
		for (auto &box : boxList_) {
			box.transform_->translate -= centorDeff;
			box.referenceBox_.max -= centorDeff;
			box.referenceBox_.min -= centorDeff;
		}
	}
	isEdited |= ImGui::DragFloat("AxisBarLength", &axisBar_->scale.x);
	if (isEdited) {
		axisBar_->UpdateMatrix();
	}

	if (goal_) {
		goal_->transform_.ImGuiWidget("Goal");
		if (ImGui::Button("DeleteGoal")) {
			goal_.reset();
		}
	}
	else {
		if (ImGui::Button("AddGoal")) {
			CreateGoal();
		}
	}

	if (ImGui::Button("AddBox")) {
		AddBox(AABB{ -Vector3::one, Vector3::one }, GroundType::kGrass);
		boxItr_ = --boxList_.end();
	}

	if (boxItr_ == boxList_.end()) {
		boxItr_ = boxList_.begin();
	}

	if (boxItr_ != boxList_.end()) {
		if (ImGui::BeginCombo("BoxList", std::to_string(std::distance(boxList_.begin(), boxItr_)).c_str())) {
			for (decltype(boxList_)::iterator it = boxList_.begin(); it != boxList_.end(); it++) {
				if (ImGui::Selectable(std::to_string(std::distance(boxList_.begin(), it)).c_str())) {
					boxItr_ = it;
					break;
				}
			}
			ImGui::EndCombo();
		}

		isEdited |= boxItr_->ImGuiWidget();
		if (boxItr_->isDelete_) {
			boxList_.erase(boxItr_);
			boxItr_ = boxList_.begin();
		}
	}

	if (ImGui::Button("AddStarItem")) {
		AddItem({});
		starItr_ = --starItem_.end();
	}

	if (starItr_ == starItem_.end()) {
		starItr_ = starItem_.begin();
	}

	if (starItr_ != starItem_.end()) {
		if (ImGui::BeginCombo("StarItemList", std::to_string(std::distance(starItem_.begin(), starItr_)).c_str())) {
			for (decltype(starItem_)::iterator it = starItem_.begin(); it != starItem_.end(); it++) {
				if (ImGui::Selectable(std::to_string(std::distance(starItem_.begin(), it)).c_str())) {
					starItr_ = it;
					break;
				}
			}
			ImGui::EndCombo();
		}

		if (ImGui::Button("DeleteStar")) {
			(*starItr_)->SetActive(false);
		}

		isEdited |= (*starItr_)->transform_.ImGuiWidget("StarItem");
		if (not(*starItr_)->GetActive()) {
			starItem_.erase(starItr_);
			starItr_ = starItem_.begin();
		}
	}

	if (isEdited) {
		this->CalcCollision();
	}

	return isEdited;
}

void LevelElementManager::Platform::CreateGoal()
{
	goal_ = std::make_unique<Entity>();

	goal_->AddComponent<GoalAnimComp>()->SetPlayerModel(LevelElementManager::GetInstance()->pPlayer_);
	goal_->transform_.SetParent(this->center_);
}

LevelElementManager::Box::Box(const AABB &aabb, Platform *parent)
{
	transform_->translate = aabb.GetCentor();
	// transform_->scale = aabb.GetRadius();
	parent_ = parent;

	referenceBox_ = aabb;
}

void LevelElementManager::Box::CreateBox(const Vector4 &color) const
{
	static auto *const blockManager = BlockManager::GetInstance();
	static auto *const levelElement = LevelElementManager::GetInstance();

	const Vector3 boxSize = referenceBox_.GetRadius();
	const Vector3 boxMin = -referenceBox_.GetRadius() + Vector3::one;

	Vector3 diff{};
	for (diff.x = 0.f; diff.x < boxSize.x; diff.x++) {
		for (diff.y = 0.f; diff.y < boxSize.y; diff.y++) {
			for (diff.z = 0.f; diff.z < boxSize.z; diff.z++) {
				IBlock block;
				block.transform_.parent_ = &transform_;

				block.transform_.translate = boxMin + diff * 2.f;
				block.transform_.scale = Vector3::one;

				blockManager->AddBox(levelElement->GetGroundModel()[static_cast<uint32_t>(this->groundType_)], std::move(block))->color_ = color;
			}
		}
	}
}

bool LevelElementManager::Box::ImGuiWidget()
{
	bool isEdited = false;

	ImGui::NewLine();
	ImGui::BulletText("BoxEditor");
	ImGui::SameLine();
	if (ImGui::Button("Delete#Box")) {
		isDelete_ = true;
		isEdited |= true;
	}

	int32_t e = static_cast<int32_t>(groundType_);
	isEdited |= ImGui::RadioButton("Grass", &e, 0);
	ImGui::SameLine();
	isEdited |= ImGui::RadioButton("Dirt", &e, 1);

	groundType_ = static_cast<GroundType>(e);

	const Vector3 beforeCentorPos = referenceBox_.GetCentor() * parent_->center_.matWorld_.GetRotate();
	Vector3 boxCentor = beforeCentorPos;
	if (ImGui::DragFloat3("BoxCentorPos", &boxCentor.x, 1.f)) {
		Vector3 move = (boxCentor - beforeCentorPos) * parent_->center_.matWorld_.InverseRT().GetRotate();
		referenceBox_ = referenceBox_.AddPos(move);
		transform_->translate += move;
		for (auto &item : transform_->translate) {
			item = std::round(item);
		}
		isEdited |= true;
	}

	const Vector3 beforeRadius = referenceBox_.GetRadius() * parent_->center_.matWorld_.GetRotate();
	Vector3 boxRadius = beforeRadius;
	if (ImGui::DragFloat3("BoxRadius", &boxRadius.x, 1.f)) {
		Vector3 radius = boxRadius * parent_->center_.matWorld_.InverseRT().GetRotate();
		for (auto &item : radius) {
			item = std::round(item);
		}
		referenceBox_ = AABB::Create(referenceBox_.GetCentor(), radius);

		isEdited |= true;
	}

	return isEdited;
}
