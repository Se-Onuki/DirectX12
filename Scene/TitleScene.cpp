#include "TitleScene.h"

#include "../externals/imgui/imgui.h"
#include "GameScene.h"
#include "../DirectBase/Base/DirectXCommon.h"
#include "../DirectBase/2D/Sprite.h"
#include "../Header/Model/Model.h"

#include "../DirectBase/Input/Input.h"
#include "../DirectBase/Base/Audio.h"

TitleScene::TitleScene() {
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
}

TitleScene::~TitleScene() {
}

void TitleScene::OnEnter()
{
	title_.reset(Sprite::Create(TextureManager::Load("UI/title.png"), MiddleCentor, TopRight));
	title_->SetPivot({ 0.5f,0.5f });

	plane_.reset(Sprite::Create(TextureManager::Load("debugfont.png"), MiddleCentor, Vector2{ 200.f,200.f }));
	plane_->SetPivot({ 0.5f,0.5f });

	light_.reset(DirectionLight::Create());
}

void TitleScene::OnExit() {
}

void TitleScene::Update()
{

	ImGui::Begin("SceneChanger");
	if (ImGui::Button("Reload : Delay 30Flame")) {
		sceneManager_->ChangeScene(new TitleScene, 30);
	}
	if (ImGui::Button("GameScene : Delay 60Flame")) {
		sceneManager_->ChangeScene(new GameScene, 60);
	}
	ImGui::End();


	ImGui::Begin("UI");
	title_->ImGuiWidget();
	ImGui::End();

	ImGui::Begin("UI2");

	const static std::array<std::string, 6u> blendMode{ "kNone", "kNormal", "kAdd", "kSubtract", "kMultily", "kScreen" };
	if (ImGui::BeginCombo("BlendPattren", blendMode[(uint32_t)blend_].c_str())) {

		for (uint32_t i = 0; i < blendMode.size(); i++) {
			if (ImGui::Selectable(blendMode[i].c_str())) {
				blend_ = (Sprite::BlendMode)i;
				break;
			}
		}
		ImGui::EndCombo();
	}

	plane_->ImGuiWidget();
	ImGui::End();

}

void TitleScene::Draw() {
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

	Model::EndDraw();

#pragma endregion

#pragma region 前面スプライト

	Sprite::StartDraw(commandList);

	title_->Draw();

	Sprite::SetBlendMode(blend_);
	plane_->Draw();

	Sprite::EndDraw();

#pragma endregion

}
