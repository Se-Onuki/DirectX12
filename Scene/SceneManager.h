#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include "../Header/Timer/Timer.h"
//#include "Scene.hpp"

class IScene {

protected:

	/// @brief 自分自身をマネージャーに登録するための関数
	//inline void SelfEntry(const std::string &name);

public:
	IScene() = default;
	virtual ~IScene() = default;

	virtual void Update() = 0;
	virtual void Draw() const = 0;

	virtual void OnEnter() = 0;
	virtual void OnExit() = 0;


	inline virtual void Delete() = 0;
};


class SceneManager {
private:

	Timer transitionTimer;

	std::unordered_map<std::string, std::unique_ptr<IScene>> scenes_;

	std::string currentSceneName;
	IScene *currentScene;

	std::string nextSceneName;
	IScene *nextScene;


	SceneManager() {
		currentScene = nullptr;
		currentSceneName = "";
	}


public:


	/// @brief インスタンスの取得
	/// @return インスタンスの実体
	inline static SceneManager *const GetInstance() {
		static SceneManager instance;
		return &instance;
	}

	/// @brief シーンの追加
	/// @param name シーン名
	/// @param scene シーンのポインタ
	inline void AddScene(const std::string &name, IScene *const scene) {
		scenes_[name].reset(scene);
	}


	/// @brief シーン遷移
	/// @param name 遷移先のシーン
	inline void ChangeScene(const std::string &name);


	/// @brief シーン遷移
	/// @param name 遷移先の名前キー
	/// @param transitionTime 必要とする時間
	inline void ChangeScene(const std::string &name, const int &transitionTime) {
		if (currentSceneName == name) return;

		if (scenes_.contains(name)) {
			nextSceneName = name;
			nextScene = scenes_[name].get();
		}
		else {
			return;
		}

		transitionTimer.Init(transitionTime);
	}


	/// @brief シーンの更新
	inline void Update() {
		if (transitionTimer.IsFinish()) {
			ChangeScene(nextSceneName);
		}

		if (currentScene) {
			currentScene->Update();
		}
		transitionTimer.Update();
	}


	/// @brief シーンの描画
	void Draw() {
		if (currentScene) {
			currentScene->Draw();
		}
	}
};