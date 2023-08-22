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

#include "../../Math/Lerp.h"
#include <algorithm>

void PlayerComp::Init() {
	input_ = Input::GetInstance();

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

	box_.reset(new Object);
	box_->Init();

	Model *const sphere = ModelManager::GetInstance()->GetModel("sphere");
	box_->AddComponent<ModelComp>()->SetModel({ {"body",{Transform{}, sphere}} });
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
		object_->transform_.rotate.y = Angle::Lerp(object_->transform_.rotate.y, moveRotate, 0.15f);
	}

	object_->transform_.ImGuiWidget();

	object_->transform_.CalcMatrix();

#pragma region 体の回転

	float &bodyRotateY = modelComp_->GetModel("body")->first.rotate.y;
	const float bodyDiff = viewProjection_->rotation_.y - object_->transform_.rotate.y;
	const float bodyEnd = Angle::Lerp(0.f, bodyDiff, 0.5f);
	bodyRotateY = Angle::Lerp(Angle::Mod(bodyRotateY), bodyEnd, 0.1f);
	ImGui::SliderAngle("bodyRotate", &bodyRotateY);

#pragma endregion

#pragma region 頭の回転

	float &headRotateY = modelComp_->GetModel("head")->first.rotate.y;
	const float headEnd = Angle::Mod(viewProjection_->rotation_.y - object_->transform_.rotate.y);

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

void PlayerComp::Draw(const ViewProjection &vp) const {

	box_->Draw(vp);
}

void PlayerComp::DrawUI() const {
	sight_->Draw();
	reticle_->Draw();
}

void PlayerComp::Attack() {
	if (input_->GetXInput()->IsTrigger(KeyCode::RIGHT_SHOULDER)) {
		if (coolTime_ == 0u) {

			AddCoolTime(fireCoolTime_);
			// カメラの回転行列
			const Matrix4x4 &cameraRotY = Matrix4x4::EulerRotate(Matrix4x4::EulerAngle::Yaw, viewProjection_->rotation_.y);
			const Vector3 &spawnPos = nozzle_ * cameraRotY + object_->GetWorldPos();

			// 弾のベクトルの生成
			Vector3 velocity = sightCentor_ - spawnPos;
			velocity = velocity.Nomalize() * bulletSpeed_;
			//const Matrix4x4& rotMat = 
			//velocity = TransformNormal(velocity, Matrix4x4::EulerRotate(viewProjection_->rotation_));

			// 弾の生成 + 初期化
			PlayerBullet *newBullet = new PlayerBullet;
			newBullet->Init();
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
	}
}

void PlayerComp::UpdateUI() {
	Vector3 sightPos = sight_->GetTransform().translate;

	const VirtualPad *const vPad = input_->GetXInput()->GetState();
	if (vPad->stickR_.Length() > 0.1f) {
		sightPos += Vector3{ vPad->stickR_.x,-vPad->stickR_.y, 0.f } *sightSpeed_;
	}
	sightPos.x = std::clamp<float>(sightPos.x, 0.f, WinApp::kWindowWidth);
	sightPos.y = std::clamp<float>(sightPos.y, 0.f, WinApp::kWindowHeight);

#pragma region 2D->3D

	const static Matrix4x4 matViewport =
		Render::MakeViewportMatrix({ 0.f,0.f }, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);
	Matrix4x4 matVPVp = viewProjection_->matView_ * viewProjection_->matProjection_ * matViewport;

	std::pair<Vector3, Vector3> segment = Render::ScreenToWorld(*(Vector2 *)&sightPos, matVPVp);

#pragma endregion

	sightCentor_ = segment.second;
	box_->transform_.translate = sightCentor_;
	box_->transform_.CalcMatrix();

	box_->Update();

	// レティクルの線分からオイラー角を生成
	Vector3 segmentRotate = Vector3{ segment.second - segment.first }.Direction2Euler();
	// オイラー角の線形補間
	followCamera_->SetRotate(Angle::Lerp(followCamera_->GetRotate(), segmentRotate, cameraRotateSpeed_));
	followCamera_->Update();

#pragma region 3D->2D

	matVPVp = viewProjection_->matView_ * viewProjection_->matProjection_ * matViewport;

	sightPos = Render::WorldToScreen(sightCentor_, matVPVp);

	sight_->SetPosition(*(Vector2 *)&sightPos);

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

		ImGui::DragFloat("sightSpeed", &sightSpeed_);
		ImGui::DragFloat("cameraRotateSpeed", &cameraRotateSpeed_, 0.01f, 0.f, 1.f);


		ImGui::TreePop();
	}
}

void PlayerComp::CoolTimeUpdate() {
	if (coolTime_) {
		coolTime_--;
	}
}
