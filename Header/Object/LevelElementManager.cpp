#include "LevelElementManager.h"
#include "../../Engine/DirectBase/Model/ModelManager.h"
#include "../../Utils/SoLib/SoLib_Easing.h"
#include "../../Utils/SoLib/SoLib_ImGui.h"
#include "../Object/Block/BlockManager.h"

#include "imgui.h"
#include "../../StarItemComp.h"
#include <numeric>
#include "../Entity/Component/PlayerComp.h"

bool LevelElementManager::AnyPlatformRotating() const {
	// どれか一つでも回転していないか
	bool anyTrue = std::any_of(this->blockCollider_.begin(), this->blockCollider_.end(), [](const auto &pair)
		{
			return pair.second.GetTimer().IsActive() == true;
		});
	return anyTrue;
}

void LevelElementManager::Undo() {
	// どれか一つでも回転していないか
	bool anyTrue = AnyPlatformRotating();
	// 回転していたら終了
	if (anyTrue) {
		return;
	}

	// もしログがあったら実行
	if (stateLog_.size()) {
		// 操作ログを末尾から1つ吐き出す
		const StateLog log = stateLog_.back();
		stateLog_.pop_back();

		// 戻すデータを登録
		undoLog_ = log;
	}
}

void LevelElementManager::UndoUpdate(const float deltaTime) {

	undoTimer_.Update(deltaTime);

	// データが登録されていたら
	if (undoLog_) {
		// もしタイマーが動作していなかったら
		if (not undoTimer_.IsActive() && AnyPlatformRotating() == false) {
			undoTimer_.Start(vLerpTime_);

			for (const auto &rotPair : undoLog_->angleList_) {
				this->GetPlatform(rotPair.first)->SetRotate(rotPair.second);
			}

		}
		// 終了したら
		if (undoTimer_.IsFinish() && undoTimer_.IsActive()) {

			pPlayer_->transform_.translate = undoLog_->item_->transform_.GetGrobalPos();
			undoLog_->item_->GetComponent<StarItemComp>()->Reset();

			undoLog_ = std::nullopt;
		}

	}
}

void LevelElementManager::ImGuiWidget() {

	ImGui::Checkbox("DebugViewer", &debugDrawer_);

	// カメラの位置は編集されたか
	bool isCameraEditedBy = false;
	isCameraEditedBy |= ImGui::DragFloat3("CameraLineStart", &lineStart_->translate.x, 1.f);
	isCameraEditedBy |= ImGui::DragFloat3("CameraLineEnd", &lineEnd_->translate.x, 1.f);
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

	static PlatformMap::iterator platformItr = blockCollider_.begin();
	std::vector<int32_t> items;
	for (const auto &pair : blockCollider_) {
		items.push_back(pair.first);
	}

	if (ImGui::BeginCombo("PlatformList", std::to_string(platformItr->first).c_str())) {

		for (PlatformMap::iterator it = blockCollider_.begin(); it != blockCollider_.end(); it++) {
			if (ImGui::Selectable(std::to_string(it->first).c_str())) {
				platformItr = it;
				break;
			}
		}
		ImGui::EndCombo();
	}

	if (platformItr->second.ImGuiWidget()) {
		this->SetTransferData();
	}

}

void LevelElementManager::AddUndoLog(Entity *const starItem) {

	std::list<std::pair<uint32_t, float>> angleList;

	for (const auto &platform : blockCollider_) {
		std::pair<uint32_t, float> keyAngle;
		keyAngle.first = platform.first;
		keyAngle.second = std::accumulate(platform.second.center_.rotate.begin(), platform.second.center_.rotate.end(), 0.f);

		angleList.push_back(keyAngle);
	}

	stateLog_.push_back(StateLog{ .item_ = starItem, .angleList_ = std::move(angleList) });

}

void LevelElementManager::Init() {
	const auto *const modelManager = ModelManager::GetInstance();
	groundModels_[static_cast<uint32_t>(GroundType::kDirt)] = modelManager->GetModel("DirtModel");
	groundModels_[static_cast<uint32_t>(GroundType::kGrass)] = modelManager->GetModel("GrassModel");

	if (lineStart_->translate == lineEnd_->translate) {
		lineEnd_->translate.z = lineStart_->translate.z + 1.f;
	}

	lineStart_->scale = Vector3::one * 0.5f;
	lineEnd_->scale = Vector3::one * 0.5f;

	lineStart_->UpdateMatrix();
	lineEnd_->UpdateMatrix();

	stageLine_.origin = lineStart_->translate;
	stageLine_.SetEnd(lineEnd_->translate);

	BlockManager::GetInstance()->clear();

	blockCollider_.clear();


	// undo処理の破棄
	stateLog_.clear();
	undoLog_ = std::nullopt;
	undoTimer_.Clear();

}

void LevelElementManager::Update([[maybe_unused]] float deltaTime) {
	UndoUpdate(deltaTime);
	for (auto &[key, platform] : blockCollider_) {
		platform.Update(deltaTime);
	}
}

void LevelElementManager::Draw([[maybe_unused]] const Camera3D &camera) const {
	//static const ModelManager *const modelManager = ModelManager::GetInstance();

	for (const auto &[key, platform] : blockCollider_) {
		// const Model *const model = groundModels_[platform.]
		platform.Draw(camera);
	}

#ifdef _DEBUG

	if (debugDrawer_) {
		this->DebugDraw(camera);
	}

#endif // _DEBUG

}
void LevelElementManager::DebugDraw([[maybe_unused]] const Camera3D &camera) const {

	const auto *const sphere = ModelManager::GetInstance()->GetModel("RedBox");

	sphere->Draw(lineStart_, camera);
	sphere->Draw(lineEnd_, camera);


}
//
//void LevelElementManager::Draw(const Camera3D &camera) {
//	/*static const ModelManager *const modelManager = ModelManager::GetInstance();
//	const Model *const model = modelManager->GetModel("Box");
//
//	for (const auto &[key, platform] : blockCollider_) {
//		platform.Draw(model, camera);
//	}*/
//}

void LevelElementManager::CalcCollision(const uint32_t key) {
	blockCollider_[key].CalcCollision();
}

void LevelElementManager::CalcCollision() {
	for (auto &[key, platform] : blockCollider_) {
		platform.CalcCollision();
	}
}

void LevelElementManager::AddBlock(const uint32_t key, const AABB &box) {
	blockCollider_[key].AddBox(box);
}

void LevelElementManager::AddItem(const uint32_t key, const BaseTransform &srt) {
	blockCollider_[key].AddItem(srt);
}

LevelElementManager::Platform *const LevelElementManager::GetPlatform(int32_t index) {
	if (index < 0) {
		return nullptr;
	}

	PlatformMap::iterator itPlatform = blockCollider_.find(index);
	if (itPlatform == blockCollider_.end()) {
		return nullptr;
	}
	return &itPlatform->second;
}

void LevelElementManager::SetTransferData() const {

	BlockManager::GetInstance()->clear();

	for (const auto &platform : this->blockCollider_) {
		for (const auto &box : platform.second.GetBoxList()) {
			box.CreateBox();
		}
	}
}

//void LevelElementManager::AddBlock(const std::string &key, Entity *const entity) {
//	blockEntity_[key].emplace_back(entity);
//}

LevelElementManager::Platform::Platform() :lerpTime_(LevelElementManager::GetInstance()->vLerpTime_) {
	startRot_ = {};
	targetRot_ = {};
}

void LevelElementManager::Platform::AddBox(const AABB &aabb) {
	boxList_.emplace_back(aabb, this);
	auto &box = boxList_.back();
	box.transform_->parent_ = &center_;

	box.CreateBox();

}

void LevelElementManager::Platform::AddItem(const BaseTransform &srt) {
	starItem_.emplace_back(std::make_unique<Entity>());
	auto *const entity = starItem_.back().get();

	//auto *const itemComp = 
	entity->AddComponent<StarItemComp>();

	entity->transform_ = srt;
	entity->transform_.parent_ = &this->center_;

}

void LevelElementManager::Platform::CalcCollision() {
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

void LevelElementManager::Platform::Update(float deltaTime) {
	timer_.Update(deltaTime);
	const Entity *const pPlayer = LevelElementManager::GetInstance()->GetPlayer();
	const auto *const playerComp = pPlayer->GetComponent<PlayerComp>();

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

void LevelElementManager::Platform::AddRotate(const float targetRot) {
	if (timer_.IsFinish()) {
		targetRot_ = Angle::Mod(rotateAxis_ * targetRot + startRot_);
		timer_.Start(lerpTime_);
	}
}

void LevelElementManager::Platform::SetRotate(const float targetRot) {
	if (timer_.IsFinish()) {
		targetRot_ = Angle::Mod(rotateAxis_ * targetRot);
		timer_.Start(lerpTime_);
	}
}

void LevelElementManager::Platform::Draw(const Camera3D &camera) const {
	/*static auto *const levelElement = LevelElementManager::GetInstance();
	for (const auto &box : boxList_) {
		levelElement->GetGroundModel()[static_cast<uint32_t>(box.groundType_)]->Draw(box.transform_, camera);
	}*/
	for (const auto &item : starItem_) {
		item->Draw(camera);
	}
}

bool LevelElementManager::Platform::ImGuiWidget() {
	ImGui::NewLine();

	ImGui::BulletText("PlatformEditor");


	bool isEdited = false;


	int32_t e;
	static std::array<Vector3, 3u> axisList = {
		Vector3::right,
		Vector3::up,
		Vector3::front
	};
	for (std::uint32_t i = 0; i < axisList.size(); ++i) {
		if (axisList[i] == rotateAxis_) {
			e = i; // 一致した場合、インデックスを返す
		}
	};
	isEdited |= ImGui::RadioButton("RotateX", &e, 0); ImGui::SameLine();
	isEdited |= ImGui::RadioButton("RotateY", &e, 1); ImGui::SameLine();
	isEdited |= ImGui::RadioButton("RotateZ", &e, 2);
	rotateAxis_ = axisList[e];

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

	static std::list<Box>::iterator boxItr = boxList_.begin();
	if (ImGui::BeginCombo("BoxList", std::to_string(reinterpret_cast<uint64_t>(static_cast<void *>(&*boxItr))).c_str())) {

		for (decltype(boxList_)::iterator it = boxList_.begin(); it != boxList_.end(); it++) {
			if (ImGui::Selectable(std::to_string(reinterpret_cast<uint64_t>(static_cast<void *>(&*it))).c_str())) {
				boxItr = it;
				break;
			}
		}
		ImGui::EndCombo();
	}

	isEdited |= boxItr->ImGuiWidget();


	if (isEdited) {
		this->CalcCollision();
	}

	return isEdited;
}

LevelElementManager::Box::Box(const AABB &aabb, Platform *parent) {
	transform_->translate = aabb.GetCentor();
	//transform_->scale = aabb.GetRadius();
	parent_ = parent;

	referenceBox_ = aabb;
}

void LevelElementManager::Box::CreateBox() const {

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

				blockManager->AddBox(levelElement->GetGroundModel()[static_cast<uint32_t>(this->groundType_)], std::move(block));

			}

		}
	}
}

bool LevelElementManager::Box::ImGuiWidget() {
	ImGui::NewLine();
	ImGui::BulletText("BoxEditor");

	bool isEdited = false;
	int32_t e = static_cast<int32_t>(groundType_);
	isEdited |= ImGui::RadioButton("Grass", &e, 0); ImGui::SameLine();
	isEdited |= ImGui::RadioButton("Dirt", &e, 1);

	groundType_ = static_cast<GroundType>(e);

	const Vector3 beforeCentorPos = referenceBox_.GetCentor() * parent_->center_.matWorld_;
	Vector3 boxCentor = beforeCentorPos;
	if (ImGui::DragFloat3("BoxCentorPos", &boxCentor.x, 1.f)) {
		Vector3 move = (boxCentor - beforeCentorPos) * parent_->center_.matWorld_.InverseRT();
		referenceBox_ = referenceBox_.AddPos(move);
		transform_->translate += move;
		isEdited |= true;
	}

	return isEdited;
}
