#include "GameScene.h"
#include "../externals/imgui/imgui.h"
#include "../DirectBase/Base/DirectXCommon.h"

GameScene::GameScene()
{
}

GameScene::~GameScene()
{
}

void GameScene::OnEnter()
{
	viewProjection.Init();

	light.reset(DirectionLight::Create());

	sprite.reset(Sprite::Create());

	//objectArray_.clear();
}

void GameScene::OnExit()
{
}

void GameScene::Update()
{
	ImGui::Begin("GameScene");
	if (ImGui::Button("Reload : Delay 30Flame")) {
		SceneManager::GetInstance()->ChangeScene(new GameScene, 30);
	}
	ImGui::End();

	ImGui::Begin("Camera");
	ImGui::DragFloat3("rotate", &viewProjection.rotation_.x, Angle::Dig2Rad);
	ImGui::DragFloat3("translate", &viewProjection.translation_.x, 0.1f);
	ImGui::End();


	ImGui::Begin("UI");
	sprite->ImGuiWidget();
	ImGui::End();


	ImGui::Begin("Light");
	light->ImGuiWidget();
	ImGui::End();

	ImGui::Begin("ObjectLoader");
	static char fileName[32];
	ImGui::InputText(".obj", fileName, 32u);
	if (ImGui::Button("Load")) {
		objectArray_.emplace_back(new Object(fileName + std::string(".obj")));
	}
#pragma region 確認用ローダ

	if (ImGui::Button("Add Plane")) {
		objectArray_.emplace_back(new Object("plane.obj"));
	}
	if (ImGui::Button("Add Sphere")) {
		objectArray_.emplace_back(new Object("sphere.obj"));
	}
	if (ImGui::Button("Add Suzanne")) {
		objectArray_.emplace_back(new Object("suzanne.obj"));
	}
	if (ImGui::Button("Add MultiMaterial")) {
		objectArray_.emplace_back(new Object("multiMaterial.obj"));
	}
	if (ImGui::Button("Add Teapot")) {
		objectArray_.emplace_back(new Object("teapot.obj"));
	}
	if (ImGui::Button("Add Bunny(Too Heavy)")) {
		objectArray_.emplace_back(new Object("bunny.obj"));
	}

#pragma endregion

	std::list<std::unique_ptr<Object>>::iterator it = objectArray_.begin();
	uint32_t index = 0;
	while (it != objectArray_.end()) {

		if (ImGui::TreeNode(((*it)->model_->name_ + "[" + std::to_string(index) + "]").c_str())) {
			(*it)->ImGuiWidget();
			ImGui::TreePop();
		}
		(*it)->transform_.UpdateMatrix();



		if (ImGui::Button(("Delete##" + std::to_string(index)).c_str())) {
			objectArray_.erase(it++); // 削除する前にイテレータをインクリメント
		}
		else {
			++it;
			index++;
		}
	}

	ImGui::End();

	viewProjection.UpdateMatrix();

	ImGui::ShowDemoWindow();

}

void GameScene::Draw()
{
	ID3D12GraphicsCommandList *const commandList = DirectXCommon::GetInstance()->GetCommandList();

	Sprite::StartDraw(commandList);
	sprite->Draw();
	Sprite::EndDraw();

	Model::StartDraw(commandList);

	light->SetLight(commandList);

	// モデルの描画

	for (auto &obj : objectArray_) {
		obj->Draw(viewProjection);
	}

	Model::EndDraw();
}
