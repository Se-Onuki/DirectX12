#include "ResultScene.h"

ResultScene::ResultScene()
{
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

}

ResultScene::~ResultScene()
{
}

void ResultScene::OnEnter()
{
}

void ResultScene::OnExit()
{
}

void ResultScene::Update()
{
	if (input_->GetDirectInput()->IsTrigger(DIK_SPACE)) {
		sceneManager_->ChangeScene("TitleScene", 0.5f);
	}
}

void ResultScene::Draw()
{
}

void ResultScene::SetGameScore(const GameScore &gameScore)
{
	gameScore_ = gameScore;
}
