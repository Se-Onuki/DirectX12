/// @file SceneManager.cpp
/// @brief シーンマネージャ
/// @author ONUKI seiya
#include "SceneManager.h"
#include "../Header/Object/Fade.h"
#include "GameScene.h"
#include "TitleScene.h"
#include "CGTaskScene.h"

SceneManager *const IScene::sceneManager_ = SceneManager::GetInstance();


void SceneManager::Init() {
	transitionTimer_.Clear();
}

void SceneManager::StaticInit() {
	sceneFactory_["TitleScene"] = []()->std::unique_ptr<IScene> { return std::make_unique<TitleScene>(); };
	sceneFactory_["GameScene"] = []()->std::unique_ptr<IScene> { return std::make_unique<GameScene>(); };
	sceneFactory_["CGTaskScene"] = []()->std::unique_ptr<IScene> { return std::make_unique<CGTaskScene>(); };
}

void SceneManager::Cancel() {
	nextScene_.reset();
	transitionTimer_.Clear();
}

void SceneManager::ChangeScene(std::unique_ptr<IScene> nextScene) {
	if (nextScene == nullptr) return;
	if (currentScene_) {
		// 遷移前のシーンの退室処理
		currentScene_->OnExit();
	}
	// 保持するシーンのキーとポインタを更新
	currentScene_ = std::move(nextScene);
	currentScene_->OnEnter();
}

void SceneManager::ChangeScene(std::unique_ptr<IScene> nextScene, const float transitionTime) {
	// もし、次のシーンがあったらキャンセル
	if (nextScene_ != nullptr) {
		return;
	}
	// 次のシーンのポインタを保存
	nextScene_ = std::move(nextScene);
	// 遷移タイマーを開始
	transitionTimer_.Start(transitionTime);
}

void SceneManager::ChangeScene(const std::string &nextScene, const float transitionTime) {

	ChangeScene(sceneFactory_.at(nextScene)(), transitionTime);
}

bool SceneManager::ImGuiWidget() {

#ifdef USE_IMGUI

	ImGui::Begin("SceneManager");
	static auto itr = sceneFactory_.begin();

	itr = SoLib::ImGuiWidget("SceneManager", &sceneFactory_, itr,
		[](const decltype(itr) &itemIndex)->std::string
		{
			return itemIndex->first;
		}
	);
	static float transitionTime = 1.f;
	ImGui::DragFloat("TransitionTime", &transitionTime, 0.1f, 0.f, 10.f, "%.3fsec");
	if (ImGui::Button("Change")) {
		if (itr != sceneFactory_.end()) {
			ChangeScene(itr->second(), transitionTime);
		}
	}
	ImGui::End();

#endif // USE_IMGUI

	return false;
}

void SceneManager::Finalize() {
	nextScene_.reset();
	if (currentScene_) { currentScene_->OnExit(); }
	currentScene_.reset();
	transitionTimer_.Clear();
}

void SceneManager::Update(float deltaTime) {

	// フェード演出の更新
	Fade::GetInstance()->Update(deltaTime);

	if (transitionTimer_.Update(deltaTime) && transitionTimer_.IsFinish()) {
		ChangeScene(std::move(nextScene_));
	}

	if (currentScene_) {
		currentScene_->Update();
	}
}

void SceneManager::PostEffectSetup()
{
	if (currentScene_) {
		currentScene_->PostEffectSetup();
	}
}

void SceneManager::PostEffectEnd()
{
	if (currentScene_) {
		currentScene_->PostEffectEnd();
	}
}

void SceneManager::Draw() const {
	if (currentScene_) {
		currentScene_->Draw();
	}
}