#include "SceneManager.h"

SceneManager *IScene::sceneManager_ = nullptr;


void SceneManager::Init() {
	IScene::sceneManager_ = this;
}

void SceneManager::ChangeScene(IScene *const nextScene) {
	if (nextScene == nullptr) return;
	if (currentScene_) {
		// 遷移前のシーンの退室処理
		currentScene_->OnExit();
	}
	// 保持するシーンのキーとポインタを更新
	currentScene_.reset(nextScene);
	currentScene_->OnEnter();
}

void SceneManager::ChangeScene(IScene *const nextScene, const int &transitionTime) {
	// 次のシーンのポインタを保存
	nextScene_.reset(nextScene);
	// 遷移タイマーを開始
	transitionTimer_.Init(transitionTime);
}

void SceneManager::Update() {
	if (transitionTimer_.Update() && transitionTimer_.IsFinish()) {
		ChangeScene(nextScene_.release());
	}

	if (currentScene_) {
		currentScene_->Update();
	}
}

void SceneManager::Draw() const {
	if (currentScene_) {
		currentScene_->Draw();
	}
}