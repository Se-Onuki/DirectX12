#include "GameScene.h"

#include <imgui.h>
#include "../Engine/DirectBase/Base/DirectXCommon.h"
#include "../Engine/DirectBase/Model/ModelManager.h"

#include "TitleScene.h"

#include "../Header/Entity/Component/ModelComp.h"
#include "../Utils/SoLib/SoLib_ImGui.h"
#include "../Engine/DirectBase/Descriptor/DescriptorHandle.h"
#include "../Header/Entity/Component/Rigidbody.h"
#include "../Header/Entity/Component/PlayerComp.h"
#include "../Header/Entity/Component/Collider.h"

GameScene::GameScene() {
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	// collisionManager_ = CollisionManager::GetInstance();
}

GameScene::~GameScene() {

}

void GameScene::OnEnter() {
	light_.reset(DirectionLight::Create());

	static auto *const modelManager = ModelManager::GetInstance();

	// モデルの読み込み
	modelManager->CreateDefaultModel(); // デフォルトモデルの読み込み
	modelManager->AddModel("Box", Model::LoadObjFile("", "box.obj"));
	modelManager->AddModel("PlayerBody", Model::LoadObjFile("Model/PlayerModel/Body/", "Body.obj")); // プレイヤーの体
	modelManager->AddModel("PlayerEye", Model::LoadObjFile("Model/PlayerModel/Eye/", "Eye.obj")); // プレイヤーの瞳
	modelManager->AddModel("PlayerHelmet", Model::LoadObjFile("Model/PlayerModel/Helmet/", "Helmet.obj")); // プレイヤーのヘルメット
	modelManager->AddModel("PlayerLing", Model::LoadObjFile("Model/PlayerModel/Ling/", "Ling.obj")); // プレイヤーの輪っか
	modelManager->AddModel("PlayerArm_L", Model::LoadObjFile("Model/PlayerModel/CharaArm/", "Arm_L.obj")); // プレイヤーの左腕
	modelManager->AddModel("PlayerArm_R", Model::LoadObjFile("Model/PlayerModel/CharaArm/", "Arm_R.obj")); // プレイヤーの右腕
	modelManager->AddModel("PlayerFoot_L", Model::LoadObjFile("Model/PlayerModel/Foot/", "Foot_L.obj")); // プレイヤーの左足
	modelManager->AddModel("PlayerFoot_R", Model::LoadObjFile("Model/PlayerModel/Foot/", "Foot_R.obj")); // プレイヤーの右足

	//model_ = ModelManager::GetInstance()->GetModel("Plane");

	//BaseTransform transform;
	//transform_ = transform;
	camera_.Init();

	/*sprite_.reset(Sprite::Create(TextureManager::Load("white2x2.png")));
	sprite_->SetScale({ 100.f,100.f });*/

	levelManager = LevelElementManager::GetInstance();

	levelManager->Init();
	levelManager->AddBlock(0u, AABB{ .min{-10.f,-1.f,-10.f}, .max{10.f,1.f,10.f} }.AddPos({ 0.f,1.f,15.f }));

	levelManager->AddBlock(0u, AABB{ .min{-10.f,-1.f,-10.f}, .max{10.f,1.f,10.f} }.AddPos({ 0.f,3.f,0.f }));
	levelManager->AddBlock(0u, AABB{ .min{-1.f,-3.f,-1.f}, .max{1.f,3.f,1.f} }.AddPos({ 0.f,5.f,0.f }));

	levelManager->blockCollider_[0u].rotate_.z = 180._deg;

	levelManager->CalcCollision(0u);

#pragma region Player

	Model *const boxModel = ModelManager::GetInstance()->GetModel("Box");
	player_ = std::make_unique<Entity>();
	//auto*const rigidbody =
	player_->AddComponent<Rigidbody>();
	auto *const modelComp =
		player_->AddComponent<ModelComp>();
	modelComp->AddBone("Body", boxModel);

	player_->AddComponent<PlayerComp>();


#pragma endregion

	playerAnim_ = std::make_unique<Entity>();
	playerAnim_->AddComponent<PlayerAnimComp>();

	//for (uint32_t i = 0u; i < transformArray_.size(); ++i) {
	//	transformArray_[i].GetCBuffer()->SetMapAddress(&instanceTransform_[i].transform);
	//	colorArray_[i].SetMapAddress(&instanceTransform_[i].color);
	//	colorArray_[i] = Vector4{ 1.f,1.f,1.f,1.f };
	//}
}

void GameScene::OnExit() {}

void GameScene::Update() {

	const float deltaTime = std::clamp(ImGui::GetIO().DeltaTime, 0.f, 0.1f);
	static auto *const colliderManager = CollisionManager::GetInstance();

	colliderManager->clear();

	ImGui::Begin("Sphere");
	//model_->ImGuiWidget();
	/*for (uint32_t i = 0u; i < transformArray_.size(); ++i) {
		if (ImGui::TreeNode(("Particle" + std::to_string(i)).c_str())) {
			transformArray_[i].ImGuiWidget();
			ImGui::ColorEdit4("Color", &colorArray_[i]->x);
			ImGui::TreePop();
		}
		transformArray_[i].UpdateMatrix();
	}*/
	ImGui::End();

	TextureManager::GetInstance()->ImGuiWindow();

	ImGui::Begin("LevelManager");
	if (ImGui::Button("Left")) {
		levelManager->blockCollider_[0u].rotate_.z += 90._deg;
	}
	if (ImGui::Button("Right")) {
		levelManager->blockCollider_[0u].rotate_.z += -90._deg;
	}
	levelManager->blockCollider_[0u].rotate_.z = Angle::Mod(levelManager->blockCollider_[0u].rotate_.z);

	ImGui::End();

	levelManager->CalcCollision(0u);
	colliderManager->push_back(levelManager->blockCollider_[0u].GetCollider());

	player_->Update(deltaTime);

	ImGui::Begin("Player");
	player_->ImGuiWidget();
	ImGui::End();

	ImGui::Begin("Camera");
	camera_.ImGuiWidget();
	ImGui::End();
	camera_.translation_ = player_->transform_.translate + Vector3{ 0.f,1.f,-15.f };
	camera_.UpdateMatrix();


	//playerAnim_->Update(deltaTime);

	light_->ImGuiWidget();

	//transform_->ImGuiWidget();
	//transform_->UpdateMatrix();

}

void GameScene::Draw()
{
	DirectXCommon *const dxCommon = DirectXCommon::GetInstance();
	ID3D12GraphicsCommandList *const commandList = dxCommon->GetCommandList();

#pragma region 背面スプライト

	Sprite::StartDraw(commandList);

	// スプライトの描画

	Sprite::EndDraw();

#pragma endregion


	dxCommon->CrearDepthBuffer();

#pragma region モデル描画

	Model::StartDraw(commandList);

	light_->SetLight(commandList);

	Model::SetPipelineType(Model::PipelineType::kModel);

	//model_->Draw(transform_, camera_);
	levelManager->Draw(camera_);

	player_->Draw(camera_);

	//Model::SetPipelineType(Model::PipelineType::kParticle);

	// モデルの描画
	// model_->Draw(instanceTransform_, camera_);

	// 描画
	//playerAnim_->Draw(camera_);

	Model::EndDraw();

#pragma endregion

#pragma region 前面スプライト

	Sprite::StartDraw(commandList);

	// スプライトの描画
	/*sprite_->Draw();*/

	Sprite::EndDraw();

#pragma endregion

}