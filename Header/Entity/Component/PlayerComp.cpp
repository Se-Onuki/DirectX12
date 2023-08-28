#include "PlayerComp.h"
#include "../../../DirectBase/Input/Input.h"
#include "../../../DirectBase/3D/ViewProjection/ViewProjection.h"
#include "Collider.h"
#include "Rigidbody.h"
#include "ModelComp.h"
#include "../PlayerBullet.h"

#include "../../../DirectBase/2D/Sprite.h"

#include "../../../Scene/GameScene.h"
#include "PlayerBulletComp.h"
#include <imgui.h>
#include "../../../DirectBase/Base/WinApp.h"
#include "../../Model/ModelManager.h"

#include "../FollowCamera.h"
#include "../../Collision/Target.h"

#include "../../Math/Lerp.h"
#include <algorithm>
#include "../../File/GlobalVariables.h"

const std::string PlayerComp::groupName = "Player";

void PlayerComp::Init() {
	input_ = Input::GetInstance();
	targeting_ = Targeting::GetInstance();
	ApplyClobalVariables();

	ColliderComp *const colliderComp = object_->AddComponent<ColliderComp>();
	colliderComp->SetCollisionAttribute(static_cast<uint32_t>(CollisionFilter::Player));
	colliderComp->SetCollisionMask(~(static_cast<uint32_t>(CollisionFilter::Player)));
	colliderComp->SetRadius(2.f);
	colliderComp->SetCentor({ 0.f,2.f,0.f });

	modelComp_ = object_->AddComponent<ModelComp>();

	const uint32_t textHaundle = TextureManager::Load("UI/circle.png");

	const Vector2 windowCentor = Vector2{ WinApp::kWindowWidth, WinApp::kWindowHeight } / 2.f;

	sight_.reset(Sprite::Create());
	sight_->SetTextureHaundle(textHaundle);
	sight_->SetColor(Vector4{ 0.f,0.f,0.f,1.f });
	sight_->SetScale(Vector2{ 1.f,1.f } *300.f);
	sight_->SetPivot({ 0.5f,0.5f });
	sight_->SetPosition(windowCentor);

	reticle_.reset(Sprite::Create());
	reticle_->SetTextureHaundle(textHaundle);
	reticle_->SetColor(Vector4{ 0.f,0.f,0.f,1.f });
	reticle_->SetScale(Vector2{ 1.f,1.f } *100.f);
	reticle_->SetPivot({ 0.5f,0.5f });
	reticle_->SetPosition(windowCentor);

	AddValue();
}

void PlayerComp::Update() {
	Rigidbody *const rigidbody = object_->GetComponent<Rigidbody>();
	rigidbody->AddAcceleration(Vector3{ 0.f,-9.8f,0.f } *2.f);

	Vector2 stickL = input_->GetXInput()->GetState()->stickL_;
	Vector3 move = Vector3{ stickL.x, 0, stickL.y };

	if (move.Length() > 0.1f) {

		//move = move.Nomalize() * moveSpeed_; // 速度を正規化
		move =                               // カメラ方向に向けて回転
			move *
			Matrix4x4::EulerRotate(Matrix4x4::EulerAngle::Yaw, viewProjection_->rotation_.y) * moveSpeed_;

		rigidbody->AddAcceleration(move); // 移動量を追加

		const float moveRotate = rigidbody->GetVelocity().Direction2Euler().y;
		object_->transform_.rotate.y = Angle::Mod(Angle::Lerp(object_->transform_.rotate.y, moveRotate, 0.15f));
	}

	object_->transform_.ImGuiWidget();

	object_->transform_.CalcMatrix();

#pragma region 体の回転

	float &bodyRotateY = modelComp_->GetBone("body")->transform_.rotate.y;
	const float bodyDiff = viewProjection_->rotation_.y - object_->transform_.rotate.y;
	const float bodyEnd = Angle::Lerp(0.f, bodyDiff, 0.5f);
	bodyRotateY = Angle::Lerp(Angle::Mod(bodyRotateY), bodyEnd, 0.1f);
	ImGui::SliderAngle("bodyRotate", &bodyRotateY);

#pragma endregion

#pragma region 頭の回転

	float &headRotateY = modelComp_->GetBone("head")->transform_.rotate.y;
	const float headEnd = Angle::Mod(viewProjection_->rotation_.y - (object_->transform_.rotate.y + bodyRotateY));

	headRotateY = Angle::Mod(Angle::Lerp(headRotateY, headEnd, 0.2f));
	ImGui::SliderAngle("headRotate", &headRotateY);

#pragma endregion

	Jump();

#pragma region UI

	UpdateUI();

#pragma endregion

	CoolTimeUpdate();

	Attack();

	const Vector3 &velocity = rigidbody->GetVelocity();
	rigidbody->SetVelocity({ velocity.x * friction_, velocity.y, velocity.z * friction_ });
}


void PlayerComp::ApplyClobalVariables() {
	GlobalVariables *const gVariable = GlobalVariables::GetInstance();

	fireCoolTime_ = gVariable->Get<int32_t>(groupName, "fireCoolTime");
	nozzle_ = gVariable->Get<Vector3>(groupName, "nozzle");
	bulletSpeed_ = gVariable->Get<float>(groupName, "bulletSpeed");

	moveSpeed_ = gVariable->Get<float>(groupName, "moveSpeed");
	friction_ = gVariable->Get<float>(groupName, "friction");
	jumpStrength_ = gVariable->Get<float>(groupName, "jumpStrength");

	sightSpeed_ = gVariable->Get<Vector2>(groupName, "sightSpeed");
	cameraRotateSpeed_ = gVariable->Get<float>(groupName, "cameraRotateSpeed");
}

void PlayerComp::AddValue() {
	GlobalVariables *const gVariable = GlobalVariables::GetInstance();

	gVariable->AddValue(groupName, "fireCoolTime", fireCoolTime_);
	gVariable->AddValue(groupName, "nozzle", nozzle_);
	gVariable->AddValue(groupName, "bulletSpeed", bulletSpeed_);

	gVariable->AddValue(groupName, "moveSpeed", moveSpeed_);
	gVariable->AddValue(groupName, "friction", friction_);
	gVariable->AddValue(groupName, "jumpStrength", jumpStrength_);

	gVariable->AddValue(groupName, "sightSpeed", sightSpeed_);
	gVariable->AddValue(groupName, "cameraRotateSpeed", cameraRotateSpeed_);
}

void PlayerComp::DrawUI() const {
	sight_->Draw();
	reticle_->Draw();
}

void PlayerComp::Attack() {
	if (input_->GetXInput()->IsPress(KeyCode::RIGHT_SHOULDER)) {
		if (coolTime_ == 0u) {

			AddCoolTime(fireCoolTime_);
			// カメラの回転行列
			const Matrix4x4 &cameraRotY = Matrix4x4::EulerRotate(Matrix4x4::EulerAngle::Yaw, viewProjection_->rotation_.y);
			const Vector3 &spawnPos = nozzle_ * cameraRotY + object_->GetWorldPos();

			// 弾のベクトルの生成
			Vector3 velocity = target_ - spawnPos;
			velocity = velocity.Nomalize() * bulletSpeed_;
			//const Matrix4x4& rotMat = 
			//velocity = TransformNormal(velocity, Matrix4x4::EulerRotate(viewProjection_->rotation_));

			// 弾の生成 + 初期化
			PlayerBullet *newBullet = new PlayerBullet;
			newBullet->Init();
			newBullet->transform_.rotate = velocity.Direction2Euler();
			auto *const bulletComp = newBullet->GetComponent<PlayerBulletComp>();
			bulletComp->SetVelocity(velocity);
			bulletComp->SetPosition(spawnPos);

			// 弾の追加
			gameScene_->AddPlayerBullet(newBullet);
		}
	}
}

void PlayerComp::Jump() {
	if (input_->GetXInput()->IsTrigger(KeyCode::LEFT_SHOULDER)) {
		Rigidbody *const rigidbody = object_->GetComponent<Rigidbody>();
		if (rigidbody->GetIsGround()) {
			rigidbody->AddAcceleration(Vector3::up() * jumpStrength_);
		}
		else {
			sightScale_ -= scaleSub_;
			rigidbody->SetVelocity(Vector3::zero());
			rigidbody->AddAcceleration((Vector3{ 0.f,-0.2f,5.f }*Matrix4x4::EulerRotate(Matrix4x4::Yaw, object_->transform_.rotate.y)) * jumpStrength_ * 2.f);
		}
	}
}

void PlayerComp::UpdateUI() {

	sightScale_ += scaleHeal_;
	sightScale_ = std::clamp(sightScale_, minSightScale_, maxSightScale_);

	sight_->SetScale(Vector2{ 2.f,2.f } *GetSightRadius());
	targeting_->SetRadius(GetSightRadius());

	Vector3 sightPos = sight_->GetTransform().translate;

	const VirtualPad *const vPad = input_->GetXInput()->GetState();
	if (vPad->stickR_.Length() > 0.1f) {
		sightPos += Vector3{ vPad->stickR_.x * sightSpeed_.x,-vPad->stickR_.y * sightSpeed_.y, 0.f };
	}
	sightPos.x = std::clamp<float>(sightPos.x, 0.f, WinApp::kWindowWidth);
	sightPos.y = std::clamp<float>(sightPos.y, 0.f, WinApp::kWindowHeight);

#pragma region 2D->3D

	const static Matrix4x4 matViewport =
		Render::MakeViewportMatrix({ 0.f,0.f }, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);
	Matrix4x4 matVPVp = viewProjection_->matView_ * viewProjection_->matProjection_ * matViewport;

	std::pair<Vector3, Vector3> segment = Render::ScreenToWorld(sightPos.ToVec2(), matVPVp);

#pragma endregion

	sightCentor_ = segment.second;
	/*box_->transform_.translate = sightCentor_;
	box_->transform_.CalcMatrix();

	box_->Update();*/

	if (vPad->stickR_.Length() <= 0.2f) {
		sightCentor_ = Lerp(sightCentor_, target_, 0.55f);
	}

	// レティクルの線分からオイラー角を生成
	Vector3 segmentDiff = Vector3{ segment.second - segment.first }.Nomalize();

	const Vector3 &segmentRotate = segmentDiff.Direction2Euler();
	Vector3 rot = Angle::Lerp(followCamera_->GetRotate(), segmentRotate, cameraRotateSpeed_);
	ImGui::DragFloat3("setRot", &rot.x);
	// オイラー角の線形補間
	followCamera_->SetRotate(rot);
	// followCamera_->Update();

#pragma region 3D->2D

	// matVPVp = viewProjection_->matView_ * viewProjection_->matProjection_ * matViewport;

	sightPos = Render::WorldToScreen(sightCentor_, matVPVp);

	sightPos.x = Lerp<float>(sightPos.x, WinApp::kWindowWidth / 2.f, 0.05f);

	sight_->SetPosition(sightPos.ToVec2());

#pragma region TargetPosition

	targeting_->SetPosition(sightPos.ToVec2());
	auto *const hitCollider = targeting_->GetHitCollider();
	if (hitCollider) {
		target_ = hitCollider->GetWorldCentor();
		reticle_->SetColor(Vector4{ 1.f,0.f,0.f,1.f });
	}
	else {
		target_ = sightCentor_;
		reticle_->SetColor(Vector4{ 0.f,0.f,0.f,1.f });
	}

#pragma endregion

	reticle_->SetPosition(Render::WorldToScreen(target_, matVPVp).ToVec2());

#pragma endregion


}

void PlayerComp::ImGuiWidget() {
	if (ImGui::TreeNode("PlayerComp")) {

		ImGui::DragInt("fireCoolTime", &fireCoolTime_, 1.f, 0, 100);
		ImGui::DragFloat3("nozzle", &nozzle_.x);
		ImGui::DragFloat("bulletSpeed", &bulletSpeed_);

		ImGui::DragFloat("moveSpeed", &moveSpeed_);
		ImGui::DragFloat("friction", &friction_);
		ImGui::DragFloat("jumpStrength", &jumpStrength_);

		ImGui::DragFloat2("sightSpeed", &sightSpeed_.x);
		ImGui::DragFloat("cameraRotateSpeed", &cameraRotateSpeed_, 0.01f, 0.f, 1.f);


		ImGui::TreePop();
	}
}

void PlayerComp::CoolTimeUpdate() {
	if (coolTime_) {
		coolTime_--;
	}
}
