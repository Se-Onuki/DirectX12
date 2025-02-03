#include "ResultScene.h"

ResultScene::ResultScene()
{
	input_ = SolEngine::Input::GetInstance();
	audio_ = SolEngine::Audio::GetInstance();
	fade_ = Fade::GetInstance();
}

ResultScene::~ResultScene()
{
}

void ResultScene::OnEnter()
{
	fade_->Start(Vector2{}, 0x00000000, 1.f);
}

void ResultScene::OnExit()
{
}

void ResultScene::Update()
{
	if (input_->GetDirectInput()->IsTrigger(DIK_SPACE)) {
		sceneManager_->ChangeScene("TitleScene", 0.5f);
		fade_->Start(Vector2{}, 0x000000FF, 0.5f);
	}
}

void ResultScene::Draw()
{
	DirectXCommon *const dxCommon = DirectXCommon::GetInstance();
	ID3D12GraphicsCommandList *const commandList = dxCommon->GetCommandList();


	Sprite::StartDraw(commandList);

	// スプライトの描画
	fade_->Draw();

	Sprite::EndDraw();

}

void ResultScene::SetGameScore(const GameScore &gameScore)
{
	gameScore_ = gameScore;
}
