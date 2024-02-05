#include "SceneManager.h"
#include "../Header/Object/Fade.h"
#include "GameScene.h"
#include "TitleScene.h"

SceneManager *const IScene::sceneManager_ = SceneManager::GetInstance();


void SceneManager::Init() {
	transitionTimer_.Clear();
}

void SceneManager::StaticInit() {
	sceneFactory_["TitleScene"] = []()->std::unique_ptr<IScene> { return std::make_unique<TitleScene>(); };
	sceneFactory_["GameScene"] = []()->std::unique_ptr<IScene> { return std::make_unique<GameScene>(); };
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
	static uint32_t index = 0u;

	SoLib::ImGuiWidget("SceneManager", &sceneFactory_, index,
		[this](uint32_t itemIndex)->std::string
		{
			// データの取得
			auto item = std::next(sceneFactory_.begin(), itemIndex);
			return item->first;
		}
	);
	static float transitionTime = 1.f;
	ImGui::DragFloat("TransitionTime", &transitionTime, 0.1f, 0.f, 10.f, "%.3fsec");
	if (ImGui::Button("Change")) {
		ChangeScene(std::next(sceneFactory_.begin(), index)->second(), transitionTime);
	}
	ImGui::End();

#endif // USE_IMGUI

	return false;
}

void SceneManager::Finalize() {
	nextScene_ = nullptr;
	currentScene_ = nullptr;
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

void SceneManager::Draw() const {
	if (currentScene_) {
		currentScene_->Draw();
	}
}