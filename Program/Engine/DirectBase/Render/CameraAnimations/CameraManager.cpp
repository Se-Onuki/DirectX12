#include "CameraManager.h"

CameraManager *CameraManager::GetInstance()
{
	static CameraManager instance;
	return &instance;
}

void CameraManager::Init()
{
	// カメラリストがnullptrなら
	if (cameraList_ == nullptr) {
		cameraList_ = std::make_unique<CameraList>();
	}

	// 読み込まれているカメラをクリア
	cameraList_->ListClear();

	// 最初のカメラを追加
	cameraList_->AddCamera("DebugCamera");
	SetUseCamera("DebugCamera");

	// カメラアニメーションマネージャーのインスタンス取得
	cameraAnimManager_ = CameraAnimManager::GetInstance(); // インスタンス取得
	cameraAnimManager_->Init();							   // 初期化
}

void CameraManager::Update(float deltaTime)
{
	// アニメーションマネージャーの更新
	cameraAnimManager_->Update(deltaTime);

	// リスト内の全てのカメラの計算を行う
	for (auto &camera : cameraList_->cameraMap_) {
		// 計算を行う
		camera.second->CalcMatrix();
	}

	// 使用カメラがあるなら
	if (useCamera_ != nullptr) {
		// 計算した内容を転送する
		useCamera_->TransferMatrix();
	}
}

void CameraManager::DisplayImGui()
{
#ifdef USE_IMGUI
	ImGui::Begin("CameraManager");
	// 全カメラののImGuiを描画
	ImGui::BeginChild(ImGui::GetID((void *)0), ImVec2(0, 150), ImGuiWindowFlags_NoTitleBar);
	// イテレータの取得
	auto iter = cameraList_->cameraMap_.begin();
	// イテレータの終わりまでループ
	while (iter != cameraList_->cameraMap_.end()) {
		// カメラ名でツリーを開始
		if (ImGui::TreeNode(iter->first.c_str())) {
			ImGui::SameLine(); // 改行しない
			// 現在使用中のカメラと一致する場合テキストで表示
			if (useCamera_ == iter->second.get()) {
				ImGui::Text("ThisCamera is used");
			}
			else { // 使用中のカメラでない場合ボタンを表示
				if (ImGui::Button("Use this")) {
					// このカメラを使用カメラに
					SetUseCamera(iter->first);
				}
			}

			// カメラの回転、座標を表示
			iter->second->ImGuiWidget("info");

			// ツリーノード終了
			ImGui::TreePop();
		}
		else {
			ImGui::SameLine(); // 改行しない

			// 現在使用中のカメラと一致する場合テキストで表示
			if (useCamera_ == iter->second.get()) {
				ImGui::Text("ThisCamera is used");
			}
			else { // 使用中のカメラでない場合ボタンを表示
				if (ImGui::Button("Use this")) {
					// このカメラを使用カメラに
					SetUseCamera(iter->first);
				}
			}
		}
		iter++;
	}
	ImGui::EndChild();

	// アニメーション再生メニュー
	ImGui::Text("AnimationMenu");
	// テキストボックス
	ImGui::InputText("SetEndCameraName", imGuiEndCameraName_, sizeof(imGuiEndCameraName_));
	// アニメーション秒数
	ImGui::DragFloat("AnimationTime", &imGuiPlayAnimTime_, 0.01f, 10.0f);
	// 現在アニメーション終了まで待機するかトリガー
	ImGui::Checkbox("StandByIsEnd", &imGuiStandByIsEnd_);
	ImGui::DragFloat("StandByTime", &imGuiStandByTime_, 0.01f, 10.0f);

	// アニメーション再生
	if (ImGui::Button("PlayAnim")) {
		std::string endCameraname = imGuiEndCameraName_;
		cameraAnimManager_->Play(cameraList_->GetCamera(endCameraname), imGuiPlayAnimTime_, SoLib::easeLinear, imGuiStandByTime_, imGuiStandByIsEnd_);
	}

	ImGui::End();
#endif // USE_IMGUI
}

Camera3D *CameraManager::AddCamera(const std::string &cameraName)
{
	// カメラリストにカメラを追加しそれを返す
	return cameraList_->AddCamera(cameraName);
}

Camera3D *CameraManager::GetCamera(const std::string &cameraName) const {
	return cameraList_->GetCamera(cameraName);
}

void CameraManager::SetUseCamera(const std::string &cameraName)
{
	// カメラ配列からカメラを取得
	Camera3D *getCamera = cameraList_->GetCamera(cameraName);
	// 取得できた場合はそのカメラを使用する
	if (getCamera != nullptr) {
		useCamera_ = getCamera;
	}
}

void CameraManager::SetUseCamera(Camera3D *camera)
{
	useCamera_ = camera;
}
