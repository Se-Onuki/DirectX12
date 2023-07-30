#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include "../Header/Timer/Timer.h"
//#include "Scene.hpp"

class IScene {
public:
	IScene() = default;
	virtual ~IScene() = default;

	virtual void OnEnter() = 0;	// シーン入室時に一度走る
	virtual void OnExit() = 0;	// シーン退室時に一度走る

	virtual void Update() = 0;	// 更新処理
	virtual void Draw() = 0;	// 描画処理
};


class SceneManager {
private:

	Timer transitionTimer{};

	std::unordered_map<std::string, std::unique_ptr<IScene>> scenes_;

	std::string currentSceneName = "";
	IScene *currentScene = nullptr;

	std::string nextSceneName = "";
	IScene *nextScene = nullptr;


	SceneManager() = default;
	SceneManager(const SceneManager &) = delete;
	SceneManager operator=(const SceneManager &) = delete;
	~SceneManager() = default;

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
	void ChangeScene(const std::string &name);


	/// @brief シーン遷移
	/// @param name 遷移先の名前キー
	/// @param transitionTime 必要とする時間
	void ChangeScene(const std::string &name, const int &transitionTime);


	/// @brief シーンの更新
	void Update();


	/// @brief シーンの描画
	void Draw() const;
};