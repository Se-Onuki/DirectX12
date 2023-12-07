#include "StarItemComp.h"
#include "Header/Entity/Component/ModelComp.h"
#include "Engine/DirectBase/Model/ModelManager.h"
#include "Header/Object/LevelElementManager.h"
#include "Engine/DirectBase/Base/Audio.h"
#include "Header/Object/TutorialManager.h"
#include "Header/Object/Particle/ParticleEmitterManager.h"

// 静的なメンバ変数の実体を宣言
uint32_t StarItemComp::collctSE_ = 0u;

void StarItemComp::Init() {
	auto *const modelComp = object_->AddComponent<ModelComp>();

	modelComp->AddBone("StarBase", ModelManager::GetInstance()->GetModel("StarBase"));
	modelComp->AddBone("Body", ModelManager::GetInstance()->GetModel("StarItem"));

	transform_->CalcMatrix();
	collider_.centor = transform_->GetGrobalPos();

	collider_.radius = 0.99f;

	if (collctSE_ == 0u) {
		collctSE_ = Audio::GetInstance()->LoadWave("resources/Audio/SE/Item/collectStar.wav");
	}
}

void StarItemComp::Reset() {
	isCollected_ = false;

	auto *const modelComp = object_->AddComponent<ModelComp>();
	modelComp->GetBone("Body")->transform_->scale = Vector3::one;

	modelComp->GetBone("StarBase")->model_ = ModelManager::GetInstance()->GetModel("StarBase");
	clearTimer_.Clear();
}

void StarItemComp::Update() {
	auto *const modelComp = object_->AddComponent<ModelComp>();

	clearTimer_.Update(object_->GetDeltaTime());
	transform_->CalcMatrix();
	collider_.centor = transform_->GetGrobalPos();
	modelComp->GetBone("Body")->transform_->rotate.y = Angle::Mod(modelComp->GetBone("Body")->transform_->rotate.y + 45._deg * object_->GetDeltaTime());

	if (clearTimer_.IsActive()) {
		modelComp->GetBone("Body")->transform_->rotate.y += 30._deg * clearTimer_.GetProgress();
		modelComp->GetBone("Body")->transform_->scale = SoLib::Lerp(Vector3::one, Vector3::zero, SoLib::easeInBack(clearTimer_.GetProgress()));
	}
}

void StarItemComp::CollectItem() {
	if (not isCollected_) {
		if (TutorialManager::GetInstance()->GetProgress() != static_cast<int32_t>(TutorialManager::TutorialProgress::kToGoal)) {
			TutorialManager::GetInstance()->SetProgress(TutorialManager::TutorialProgress::kFloatZ);
		}
		auto *const modelComp = object_->AddComponent<ModelComp>();
		modelComp->GetBone("StarBase")->model_ = ModelManager::GetInstance()->GetModel("StarBaseShine");

		clearTimer_.Start(1.f);

		ParticleEmitter *pEmitter_ = ParticleEmitterManager::GetInstance()->CreateEmitter<GetParticle>("StarItem", 0.2f);
		pEmitter_->emitTransform_ = *transform_;

		// undoログにデータを追加
		LevelElementManager::GetInstance()->AddUndoLog(object_);
		LevelElementManager::GetInstance()->AddStarCount(1);

		Audio::GetInstance()->PlayWave(collctSE_, false, 0.5f);

		isCollected_ = true;
	}
}
