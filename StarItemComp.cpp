#include "StarItemComp.h"
#include "Header/Entity/Component/ModelComp.h"
#include "Engine/DirectBase/Model/ModelManager.h"
#include "Header/Object/LevelElementManager.h"
#include "Engine/DirectBase/Base/Audio.h"

// 静的なメンバ変数の実体を宣言
uint32_t StarItemComp::collctSE_ = 0u;

void StarItemComp::Init() {
	auto *const modelComp = object_->AddComponent<ModelComp>();

	modelComp->AddBone("Body", ModelManager::GetInstance()->GetModel("StarItem"));

	transform_->CalcMatrix();
	collider_.centor = transform_->GetGrobalPos();

	collider_.radius = 1.5f;

	if (collctSE_ == 0u) {
		collctSE_ = Audio::GetInstance()->LoadWave("resources/Audio/SE/Item/collectStar.wav");
	}
}

void StarItemComp::Reset() {
	isCollected_ = false;

	auto *const modelComp = object_->AddComponent<ModelComp>();
	modelComp->GetBone("Body")->transform_->scale = Vector3::one;
}

void StarItemComp::Update() {
	auto *const modelComp = object_->AddComponent<ModelComp>();

	transform_->CalcMatrix();
	collider_.centor = transform_->GetGrobalPos();
	modelComp->GetBone("Body")->transform_->rotate.y = Angle::Mod(modelComp->GetBone("Body")->transform_->rotate.y + 45._deg * object_->GetDeltaTime());

}

void StarItemComp::CollectItem() {
	if (not isCollected_) {
		auto *const modelComp = object_->AddComponent<ModelComp>();
		modelComp->GetBone("Body")->transform_->scale = Vector3::zero;

		// undoログにデータを追加
		LevelElementManager::GetInstance()->AddUndoLog(object_);
		LevelElementManager::GetInstance()->AddRotateCount(1);

		Audio::GetInstance()->PlayWave(collctSE_, false, 0.5f);

		isCollected_ = true;
	}
}
