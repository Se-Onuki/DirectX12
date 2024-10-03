#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include "../../Engine/Utils/SoLib/SoLib_Timer.h"
#include "../Engine/DirectBase/Base/DirectXCommon.h"
#include "../Engine/DirectBase/Base/EngineObject.h"
//#include "Scene.hpp"

class SceneManager;
class GameObject;

class IScene : public SolEngine::EngineObject {
public:
	IScene() = default;
	virtual ~IScene() = default;

	virtual void OnEnter() = 0;	// シーン入室時に一度走る
	virtual void OnExit() = 0;	// シーン退室時に一度走る

	virtual void Update() = 0;	// 更新処理
	virtual void PostEffectSetup() { DirectXCommon::GetInstance()->DefaultDrawReset(); }	// ポストエフェクトの設定処理
	virtual void PostEffectEnd() {  }	// ポストエフェクトの終了処理
	virtual void Draw() = 0;	// 描画処理

	static SceneManager *const sceneManager_;

	/// 今後実装
	// void AddObject(Object*const object);
	// std::list<std::unique_ptr<Object>> objectList_;
};

class SceneManager : public SolEngine::EngineObject {
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

	/// @brief 文字列をキーとしてシーンの生成関数を保存する
	std::unordered_map<std::string, std::unique_ptr<IScene>(*)()> sceneFactory_;

public:

	void Init();

	/// @brief インスタンスの取得
	/// @return インスタンスの実体
	inline static SceneManager *const GetInstance() {
		static SceneManager instance;
		return &instance;
	}

	void StaticInit();

	void Cancel();


	/// @brief シーン遷移
	/// @param name 遷移先のシーン
	void ChangeScene(std::unique_ptr<IScene> nextScene);


	/// @brief シーン遷移
	/// @param name 遷移先の名前キー
	/// @param transitionTime 必要とする時間
	void ChangeScene(std::unique_ptr<IScene> nextScene, const float transitionTime);

	template<SoLib::IsBased<IScene> T>
	void ChangeScene(const float transitionTime);

	void ChangeScene(const std::string &nextScene, const float transitionTime = 0.f);

	bool ImGuiWidget();

	void Finalize();


	/// @brief シーンの更新
	void Update(float deltaTime);

	/// @brief ポストエフェクトの設定処理
	void PostEffectSetup();

	/// @brief ポストエフェクトの終了処理
	void PostEffectEnd();

	/// @brief シーンの描画
	void Draw() const;
};

template<SoLib::IsBased<IScene> T>
inline void SceneManager::ChangeScene(const float transitionTime)
{
	ChangeScene(std::make_unique<T>(), transitionTime);
}
