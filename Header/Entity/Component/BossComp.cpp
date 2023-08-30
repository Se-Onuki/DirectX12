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

	//const uint32_t textHaundle = TextureManager::Load("UI/circle.png");
	vBarCentor_ = Vector2{ WinApp::kWindowWidth * 0.5f, WinApp::kWindowHeight * 0.25f };

	healthBarFrame_.reset(Sprite::Create());
	//healthBarFrame_->SetTextureHaundle(textHaundle);
	healthBarFrame_->SetColor(Vector4{ 1.f,1.f,1.f,0.5f });
	healthBarFrame_->SetScale(vBarScale_);
	healthBarFrame_->SetPivot({ 0.5f,0.5f });
	healthBarFrame_->SetPosition(vBarCentor_);


	healthBar_.reset(Sprite::Create());
	healthBar_->SetColor(Vector4{ 1.f,0.f,0.f,1.f });
	healthBar_->SetScale(Vector2{ vBarScale_->x / 2.f, vBarScale_->y });
	healthBar_->SetPivot({ 0.0f,0.5f });
	healthBar_->SetPosition(vBarCentor_.GetItem() - Vector2{ vBarScale_->x / 2.f,0.f });



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


	auto *const healthComp = object_->GetComponent<HealthComp>();
	float healthProgress = healthComp->GetProgress();
	healthBar_->SetScale(Vector2{ vBarScale_->x * healthProgress, vBarScale_->y });
}

void BossComp::DrawUI() const {
	healthBar_->Draw();
	healthBarFrame_->Draw();
}

void BossComp::ApplyVariables(const char *const groupName) {
	GlobalVariables *const gVariable = GlobalVariables::GetInstance();

	gVariable->GetGroup(groupName) >> vRadius_;
	gVariable->GetGroup(groupName) >> vMaxHealth_;

	gVariable->GetGroup(groupName) >> vBarCentor_;
	gVariable->GetGroup(groupName) >> vBarScale_;
}

void BossComp::AddVariable(const char *const groupName) const {
	GlobalVariables *const gVariable = GlobalVariables::GetInstance();

	gVariable->AddValue(groupName, vRadius_);
	gVariable->AddValue(groupName, vMaxHealth_);

	gVariable->AddValue(groupName, vBarCentor_);
	gVariable->AddValue(groupName, vBarScale_);
}

void BossComp::SetGameScene(GameScene *const gameScene) {
	gameScene_ = gameScene;
}
