#include "SceneManager.h"

inline void SceneManager::ChangeScene(const std::string &name) {
	// 現在のシーンと同じであった場合、実行しない。
	if (currentSceneName == name) return;
	// マップに一致する要素があった場合、実行。
	if (scenes_.contains(name)) {
		// 現在のシーンのキーとポインタを更新
		currentSceneName = name;
		currentScene = scenes_[name].get();
		currentScene->OnEnter();
	}
}
