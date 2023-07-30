#include "SceneManager.h"

void SceneManager::ChangeScene(const std::string &name) {
	// 現在のシーンと同じであった場合、実行しない。
	//if (currentSceneName == name) return;
	// マップに一致する要素があった場合、実行。
	if (scenes_.contains(name)) {
		if (currentScene) {
			// 遷移前処理
			currentScene->OnExit();
		}
		// 保持するシーンのキーとポインタを更新
		currentSceneName = name;
		currentScene = scenes_[name].get();
		currentScene->OnEnter();
	}
}

void SceneManager::ChangeScene(const std::string &name, const int &transitionTime) {
	//if (currentSceneName == name) return;

	if (scenes_.contains(name)) {
		nextSceneName = name;
		nextScene = scenes_[name].get();
	}
	else {
		return;
	}

	transitionTimer.Init(transitionTime);
}

void SceneManager::Update() {
	if (transitionTimer.Update() && transitionTimer.IsFinish()) {
		ChangeScene(nextSceneName);
	}

	if (currentScene) {
		currentScene->Update();
	}
}

void SceneManager::Draw() const {
	if (currentScene) {
		currentScene->Draw();
	}
}