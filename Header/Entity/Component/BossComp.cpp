#include "BossComp.h"
#include "Collider.h"
#include "ModelComp.h"
#include "HealthComp.h"
#include "../../Model/ModelManager.h"

#include "../../File/GlobalVariables.h"
#include "../../../DirectBase/Base/SoLib.h"

#include "../../Math/Transform.h"

#include "../../../Scene/GameScene.h"
#include "../Player.h"
#include "../../../DirectBase/Base/WinApp.h"

const char *const BossComp::groupName_ = "Boss";

void BossComp::Init() {
	ApplyVariables(groupName_);

	auto *const colliderComp = object_->AddComponent<ColliderComp>();
	auto *const modelComp = object_->AddComponent<ModelComp>();
	auto *const healthComp = object_->AddComponent<HealthComp>();

	colliderComp->SetCollisionAttribute(static_cast<uint32_t>(CollisionFilter::Enemy));
	colliderComp->SetCollisionMask(~(static_cast<uint32_t>(CollisionFilter::Enemy)));
	colliderComp->SetRadius(vRadius_);
	colliderComp->SetCentor(Vector3::up() * vRadius_);

	Model *const enemyModel = ModelManager::GetInstance()->GetModel("sphere");
	modelComp->AddBone("body", enemyModel, Transform{ .translate{0.f,vRadius_,0.f} });

	healthComp->SetMaxHealth(vMaxHealth_);
	healthComp->Reset();

	const uint32_t textHaundle = TextureManager::Load("UI/BossBarFlame.png");
	const auto &texDesc = TextureManager::GetInstance()->GetResourceDesc(textHaundle);
	const Vector2 &textureSize = { static_cast<float>(texDesc.Width), static_cast<float>(texDesc.Height) };

	barFlameSize_ = { vBarScale_->x * (vBarFlame_->x / textureSize.x), vBarScale_->y * (vBarFlame_->y / textureSize.y) };

	healthBarFrame_.reset(Sprite::Create());
	healthBarFrame_->SetTextureHaundle(textHaundle);
	healthBarFrame_->SetColor(Vector4{ 1.f,1.f,1.f,1.f });
	healthBarFrame_->SetScale(vBarScale_);
	healthBarFrame_->SetPivot({ 0.5f,0.5f });
	healthBarFrame_->SetPosition(vBarCentor_);


	healthBar_.reset(Sprite::Create());
	healthBar_->SetColor(Vector4{ 0.f,1.f,0.f,1.f });
	healthBar_->SetScale(vBarScale_.GetItem() - barFlameSize_ * 2.f);
	healthBar_->SetPivot({ 0.0f,0.5f });
	healthBar_->SetPosition(vBarCentor_.GetItem() - Vector2{ vBarScale_->x * 0.5f - barFlameSize_.x,0.f });


	AddVariable(groupName_);

}

void BossComp::Update() {
	auto *const player = gameScene_->GetPlayer();

	if (player) {
		target_ = player->GetWorldPos();
	}

	Vector3 targetDiff = target_ - object_->GetWorldPos();

	Vector3 bossRot = targetDiff.Direction2Euler();
	object_->transform_.rotate.y = bossRot.y;


	HealthComp *const healthComp = object_->GetComponent<HealthComp>();
	const float healthProgress = healthComp->GetProgress();
	healthBar_->SetScale(Vector2{ (vBarScale_->x - barFlameSize_.x * 2.f) * healthProgress , vBarScale_->y - barFlameSize_.y * 2.f });
}

void BossComp::DrawUI() const {
	healthBarFrame_->Draw();
	healthBar_->Draw();
}

void BossComp::ApplyVariables(const char *const groupName) {
	GlobalVariables *const gVariable = GlobalVariables::GetInstance();

	auto &group = gVariable->GetGroup(groupName);

	group >> vRadius_;
	group >> vMaxHealth_;

	group >> vBarCentor_;
	group >> vBarScale_;

	group >> vBarFlame_;
}

void BossComp::AddVariable(const char *const groupName) const {
	GlobalVariables *const gVariable = GlobalVariables::GetInstance();

	gVariable->AddValue(groupName, vRadius_);
	gVariable->AddValue(groupName, vMaxHealth_);

	gVariable->AddValue(groupName, vBarCentor_);
	gVariable->AddValue(groupName, vBarScale_);

	gVariable->AddValue(groupName, vBarFlame_);
}

void BossComp::SetGameScene(GameScene *const gameScene) {
	gameScene_ = gameScene;
}
