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

GameScene::GameScene() {
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	// collisionManager_ = CollisionManager::GetInstance();
}

GameScene::~GameScene() {

}

void GameScene::OnEnter() {
	light_.reset(DirectionLight::Create());

	// モデルの読み込み
	ModelManager::GetInstance()->CreateDefaultModel(); // デフォルトモデルの読み込み
	ModelManager::GetInstance()->AddModel("Box", Model::LoadObjFile("", "box.obj"));
	ModelManager::GetInstance()->AddModel("PlayerBody", Model::LoadObjFile("Model/Player/Body/", "Body.obj")); // プレイヤーの体
	ModelManager::GetInstance()->AddModel("PlayerEye", Model::LoadObjFile("Model/Player/Eye/", "Eye.obj")); // プレイヤーの瞳
	ModelManager::GetInstance()->AddModel("PlayerHelmet", Model::LoadObjFile("Model/Player/Helmet/", "Helmet.obj")); // プレイヤーのヘルメット
	ModelManager::GetInstance()->AddModel("PlayerLing", Model::LoadObjFile("Model/Player/Ling/", "Ling.obj")); // プレイヤーの輪っか
	ModelManager::GetInstance()->AddModel("PlayerArm_L", Model::LoadObjFile("Model/Player/Arm/", "Arm_L.obj")); // プレイヤーの左腕
	ModelManager::GetInstance()->AddModel("PlayerArm_R", Model::LoadObjFile("Model/Player/Arm/", "Arm_R.obj")); // プレイヤーの右腕
	ModelManager::GetInstance()->AddModel("PlayerFoot_L", Model::LoadObjFile("Model/Player/Foot/", "Foot_L.obj")); // プレイヤーの左足
	ModelManager::GetInstance()->AddModel("PlayerFoot_R", Model::LoadObjFile("Model/Player/Foot/", "Foot_R.obj")); // プレイヤーの右足

	/*model_ = ModelManager::GetInstance()->GetModel("Plane");
	transform_.UpdateMatrix();*/
	camera_.Init();

	/*sprite_.reset(Sprite::Create(TextureManager::Load("white2x2.png")));
	sprite_->SetScale({ 100.f,100.f });*/

	levelManager = LevelElementManager::GetInstance();

	levelManager->Init();
	levelManager->AddBlock(0u, AABB{ .min{-3.f,-1.f,-3.f}, .max{3.f,1.f,3.f} }.AddPos({ 0.f,3.f,0.f }));
	levelManager->AddBlock(0u, AABB{ .min{-1.f,-3.f,-1.f}, .max{1.f,3.f,1.f} }.AddPos({ 0.f,5.f,0.f }));

	levelManager->blockCollider_[0u].rotate_.z = 180._deg;

	levelManager->CalcCollision(0u);

#pragma region Player

	//Model *const boxModel = ModelManager::GetInstance()->GetModel("Box");
	//player_ = std::make_unique<Entity>();
	////auto*const rigidbody =
	//player_->AddComponent<Rigidbody>();
	//auto *const modelComp =
	//	player_->AddComponent<ModelComp>();
	//modelComp->AddBone("Body", playerBodyModel);
	//modelComp->AddBone("Body", playerHelmetModel);

	//player_->AddComponent<PlayerComp>();


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

	const float deltaTime = ImGui::GetIO().DeltaTime;

	ImGui::Begin("Camera");
	camera_.ImGuiWidget();
	ImGui::End();
	camera_.UpdateMatrix();

	ImGui::Begin("Sphere");
	/*model_->ImGuiWidget();*/
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

	//player_->Update(deltaTime);

	playerAnim_->Update(deltaTime);

	light_->ImGuiWidget();

	/*transform_.ImGuiWidget();
	transform_.UpdateMatrix();*/
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

	/*model_->Draw(transform_, camera_);*/
	/*levelManager->Draw(camera_);*/

	//player_->Draw(camera_);

	//Model::SetPipelineType(Model::PipelineType::kParticle);

	// モデルの描画
	// model_->Draw(instanceTransform_, camera_);

	// 描画
	playerAnim_->Draw(camera_);

	Model::EndDraw();

#pragma endregion

#pragma region 前面スプライト

	Sprite::StartDraw(commandList);

	// スプライトの描画
	/*sprite_->Draw();*/

	Sprite::EndDraw();

#pragma endregion

}