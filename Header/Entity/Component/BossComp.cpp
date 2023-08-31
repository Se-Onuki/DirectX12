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
#include "../BossBullet.h"

const char *const BossComp::groupName_ = "Boss";

void BossComp::Init() {
	ApplyVariables(groupName_);

	auto *const colliderComp = object_->AddComponent<ColliderComp>();
	auto *const modelComp = object_->AddComponent<ModelComp>();
	healthComp_ = object_->AddComponent<HealthComp>();

	colliderComp->SetCollisionAttribute(static_cast<uint32_t>(CollisionFilter::Enemy));
	colliderComp->SetCollisionMask(~(static_cast<uint32_t>(CollisionFilter::Enemy | CollisionFilter::Bullet)));
	colliderComp->SetRadius(vRadius_);
	colliderComp->SetCentor(Vector3::up() * vRadius_);

	Model *const enemyModel = ModelManager::GetInstance()->GetModel("sphere");
	modelComp->AddBone("body", enemyModel, Transform{ .scale{Vector3::one() * vRadius_.GetItem()}, .translate{0.f,vRadius_,0.f} });

	healthComp_->SetMaxHealth(vMaxHealth_);
	healthComp_->Reset();

	const uint32_t textHaundle = TextureManager::Load("UI/BossBarFlame.png");
	const auto &texDesc = TextureManager::GetInstance()->GetResourceDesc(textHaundle);
	const Vector2 &textureSize = { static_cast<float>(texDesc.Width), static_cast<float>(texDesc.Height) };

	barFlameSize_ = { vBarScale_->x * (vBarFlame_->x / textureSize.x), vBarScale_->y * (vBarFlame_->y / textureSize.y) };

	bossText_.reset(Sprite::Create(TextureManager::Load("UI/enemyText.png")));
	bossText_->SetColor(Vector4{ 1.f,1.f,1.f,1.f });
	bossText_->SetScale(Vector2{ 512.f,128.f }*0.5f);
	bossText_->SetPivot({ 0.5f,0.5f });
	bossText_->SetPosition(-Vector2{ 0.f,48.f } + vBarCentor_);

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
	CoolTimeUpdate();
	auto *const player = gameScene_->GetPlayer();

	if (player) {
		target_ = player->GetComponent<ColliderComp>()->GetGlobalCentor();
	}

	Vector3 targetDiff = target_ - object_->GetWorldPos();

	Vector3 bossRot = targetDiff.Direction2Euler();
	object_->transform_.rotate.y = bossRot.y;

	Attack();

	const float healthProgress = healthComp_->GetProgress();
	healthBar_->SetScale(Vector2{ (vBarScale_->x - barFlameSize_.x * 2.f) * healthProgress , vBarScale_->y - barFlameSize_.y * 2.f });
}

void BossComp::DrawUI() const {
	healthBarFrame_->Draw();
	healthBar_->Draw();
	bossText_->Draw();
}

void BossComp::ApplyVariables(const char *const groupName) {
	GlobalVariables *const gVariable = GlobalVariables::GetInstance();

	auto &group = gVariable->GetGroup(groupName);

	group >> vRadius_;
	group >> vMaxHealth_;

	group >> vBarCentor_;
	group >> vBarScale_;

	group >> vBarFlame_;
	group >> vFireCoolTime_;
	group >> vRangeCoolTime_;

	group >> vBulletSpeed_;
	group >> vNozzle_;

	group >> vBulletLifeTime_;
	group >> vRangeLifeTime_;

	group >> vRangeBulletCount_;
	group >> vRangeAngle_;
	group >> vRangeBulletSpeed_;
}

void BossComp::AddVariable(const char *const groupName) const {
	GlobalVariables *const gVariable = GlobalVariables::GetInstance();

	gVariable->AddValue(groupName, vRadius_);
	gVariable->AddValue(groupName, vMaxHealth_);

	gVariable->AddValue(groupName, vBarCentor_);
	gVariable->AddValue(groupName, vBarScale_);

	gVariable->AddValue(groupName, vBarFlame_);
	gVariable->AddValue(groupName, vFireCoolTime_);
	gVariable->AddValue(groupName, vRangeCoolTime_);

	gVariable->AddValue(groupName, vBulletSpeed_);
	gVariable->AddValue(groupName, vNozzle_);

	gVariable->AddValue(groupName, vBulletLifeTime_);
	gVariable->AddValue(groupName, vRangeLifeTime_);

	gVariable->AddValue(groupName, vRangeBulletCount_);
	gVariable->AddValue(groupName, vRangeAngle_);
	gVariable->AddValue(groupName, vRangeBulletSpeed_);
}

void BossComp::SetGameScene(GameScene *const gameScene) {
	gameScene_ = gameScene;
}

void BossComp::CoolTimeUpdate() {
	if (coolTime_ > 0) {
		--coolTime_;
	}
}

void BossComp::MoveBehavior() {
}

void BossComp::Attack() {
	if (coolTime_ == 0u) {

		AddCoolTime(vFireCoolTime_);
		// カメラの回転行列
		const Matrix4x4 &rotateY = Matrix4x4::EulerRotate(Matrix4x4::EulerAngle::Yaw, object_->transform_.rotate.y);
		const Vector3 &spawnPos = vNozzle_.GetItem() * rotateY + object_->GetWorldPos();

		// 弾のベクトルの生成
		Vector3 velocity = target_ - spawnPos;
		velocity = velocity.Nomalize() * vBulletSpeed_;

		// 弾の生成 + 初期化
		FireBullet(spawnPos, velocity, vBulletLifeTime_);
	}
}

void BossComp::AttackRange() {
	if (coolTime_ == 0u) {

		AddCoolTime(vRangeCoolTime_);

		const Vector3 &spawnPos = object_->GetWorldPos() + Vector3::up();


		// 弾のベクトルの生成
		// const float vRangeDivisions_ = 64.f;
		// const float vRangeAngle_ = 45.f;
		const float rangeDiff = vRangeAngle_ * Angle::Dig2Rad;
		for (int32_t i = 0; i < vRangeBulletCount_.GetItem(); i++) {
			const Matrix4x4 &rotateY = Matrix4x4::EulerRotate(Matrix4x4::EulerAngle::Yaw, rangeDiff / vRangeBulletCount_ * i + object_->transform_.rotate.y - rangeDiff * 0.5f);
			const Vector3 &velocity = (Vector3::front() * rotateY) * vRangeBulletSpeed_;

			// 弾の生成 + 初期化
			FireBullet(spawnPos, velocity, vRangeLifeTime_);

		}
	}
}

void BossComp::FireBullet(const Vector3 &spawnPos, const Vector3 &velocity, const int32_t lifeTime) {

	// 弾の生成 + 初期化
	BossBullet *newBullet = new BossBullet;
	newBullet->Init();
	newBullet->transform_.rotate = velocity.Direction2Euler();
	auto *const bulletComp = newBullet->GetComponent<BossBulletComp>();
	bulletComp->SetVelocity(velocity);
	bulletComp->SetPosition(spawnPos);
	bulletComp->SetLifeTime(lifeTime);

	// 弾の追加
	gameScene_->AddBossBullet(newBullet);
}
