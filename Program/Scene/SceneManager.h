#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include "../Utils/SoLib/SoLib_Timer.h"
//#include "Scene.hpp"

class SceneManager;
class Entity;

class IScene {
public:
	IScene() = default;
	virtual ~IScene() = default;

	virtual void OnEnter() = 0;	// シーン入室時に一度走る
	virtual void OnExit() = 0;	// シーン退室時に一度走る

	virtual void Update() = 0;	// 更新処理
	virtual void Draw() = 0;	// 描画処理

	static SceneManager *sceneManager_;

	/// 今後実装
	// void AddObject(Object*const object);
	// std::list<std::unique_ptr<Object>> objectList_;
};

template<typename T>
concept IsBasedIScene = std::is_base_of<IScene, T>::value;


class SceneManager {
private:

	SoLib::DeltaTimer transitionTimer_{};
	// 現在読み込んでいるシーン
	std::unique_ptr<IScene> currentScene_ = nullptr;
	// 次に遷移するシーン
	std::unique_ptr<IScene> nextScene_ = nullptr;


	SceneManager() = default;
	SceneManager(const SceneManager &) = delete;
	SceneManager operator=(const SceneManager &) = delete;
	~SceneManager() = default;

public:

	void Init();


	/// @brief インスタンスの取得
	/// @return インスタンスの実体
	inline static SceneManager *const GetInstance() {
		static SceneManager instance;
		return &instance;
	}

	void Cancel();


	/// @brief シーン遷移
	/// @param name 遷移先のシーン
	void ChangeScene(std::unique_ptr<IScene> nextScene);


	/// @brief シーン遷移
	/// @param name 遷移先の名前キー
	/// @param transitionTime 必要とする時間
	void ChangeScene(std::unique_ptr<IScene> nextScene, const float transitionTime);


	/// @brief シーンの更新
	void Update(float deltaTime);


	/// @brief シーンの描画
	void Draw() const;
};