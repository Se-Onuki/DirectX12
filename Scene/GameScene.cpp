#include "GameScene.h"
#include "../externals/imgui/imgui.h"
#include "../DirectBase/Base/DirectXCommon.h"
#include "TitleScene.h"

GameScene::GameScene()
{
}

GameScene::~GameScene()
{
}

void GameScene::OnEnter()
{
	viewProjection.Init();
	viewProjection.translation_ = { 0.f,0.f,-15.f };

	light_.reset(DirectionLight::Create());

	sprite_.reset(Sprite::Create(TextureManager::Load("uvChecker.png"), Vector2{ 100.f,100.f }, Vector2{ 100.f,100.f }));

	//objectArray_.clear();
}

void GameScene::OnExit()
{
}

void GameScene::Update()
{
	ImGui::Begin("SceneChanger");
	if (ImGui::Button("Reload : Delay 30Flame")) {
		SceneManager::GetInstance()->ChangeScene(new GameScene, 30);
	}
	if (ImGui::Button("TitleScene : Delay 60Flame")) {
		SceneManager::GetInstance()->ChangeScene(new TitleScene, 60);
	}
	ImGui::End();

	TextureManager::GetInstance()->ImGuiWindow();

	ImGui::Begin("Camera");
	ImGui::DragFloat3("rotate", &viewProjection.rotation_.x, Angle::Dig2Rad);
	ImGui::DragFloat3("translate", &viewProjection.translation_.x, 0.1f);
	ImGui::End();


	ImGui::Begin("UI");
	sprite_->ImGuiWidget();
	ImGui::End();


	ImGui::Begin("Light");
	light_->ImGuiWidget();
	ImGui::End();

	ImGui::Begin("ObjectLoader");
	static char fileName[32];
	ImGui::InputText(".obj", fileName, 32u);
	if (ImGui::Button("Load")) {
		objectList_.emplace_back(new Object(fileName + std::string(".obj")));
	}

#pragma region モデル確認用ローダ

	if (ImGui::Button("Add Plane")) {
		objectList_.emplace_back(new Object("plane.obj"));
	}
	if (ImGui::Button("Add Sphere")) {
		objectList_.emplace_back(new Object("sphere.obj"));
	}
	if (ImGui::Button("Add Suzanne")) {
		objectList_.emplace_back(new Object("suzanne.obj"));
	}
	if (ImGui::Button("Add MultiMaterial")) {
		objectList_.emplace_back(new Object("multiMaterial.obj"));
	}
	if (ImGui::Button("Add Teapot")) {
		objectList_.emplace_back(new Object("teapot.obj"));
	}
	if (ImGui::Button("Add Bunny(Too Heavy)")) {
		objectList_.emplace_back(new Object("bunny.obj"));
	}

#pragma endregion


	ImGui::Text("inspector");
	std::list<std::unique_ptr<Object>>::iterator it = objectList_.begin();
	uint32_t index = 0;
	while (it != objectList_.end()) {

		if (ImGui::TreeNode(((*it)->model_->name_ + "[" + std::to_string(index) + "]").c_str())) {
			(*it)->ImGuiWidget();
			ImGui::TreePop();
		}
		(*it)->transform_.UpdateMatrix();


		if (ImGui::Button(("Delete##" + std::to_string(index)).c_str())) {
			objectList_.erase(it++); // 削除する前にイテレータをインクリメント
		}
		else {
			it++;
			index++;
		}
	}

	ImGui::End();

	viewProjection.UpdateMatrix();

	ImGui::ShowDemoWindow();

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

	// モデルの描画

	for (auto &obj : objectList_) {
		obj->Draw(viewProjection);
	}


	Model::EndDraw();

#pragma endregion

#pragma region 前面スプライト

	Sprite::StartDraw(commandList);

	// スプライトの描画
	sprite_->Draw();

	Sprite::EndDraw();

#pragma endregion

}
