#include "LoadScene.h"
#include "TitleScene.h"
#include "../Header/Object/Fade.h"

LoadScene::LoadScene()
{
	// インスタンス取得
	audio_ = Audio::GetInstance(); // 音
	sceneManager_ = SceneManager::GetInstance(); // シーンマネージャー
}

LoadScene::~LoadScene()
{
}

void LoadScene::OnEnter()
{
	// インスタンス取得
	modelManager_ = ModelManager::GetInstance();	 // モデル

	// 3Dモデルの読み込み
	modelManager_->CreateDefaultModel(); // デフォルトモデルの読み込み
	modelManager_->AddModel("Box", Model::LoadObjFile("", "box.obj"));
	modelManager_->AddModel("RedBox", Model::LoadObjFile("", "box.obj"))->materialMap_["Material"]->materialBuff_->color = Vector4{ 1.f,0.f,0.f,1.f };
	modelManager_->AddModel("GrassModel", Model::LoadObjFile("Model/Ground/", "Ground.obj")); // 草ブロック
	modelManager_->AddModel("DirtModel", Model::LoadObjFile("Model/Ground/", "Ground.obj"))->materialMap_.begin()->second->texHandle_ = TextureManager::Load("Model/Ground/DustTex.png"); // 土ブロック
	modelManager_->AddModel("PlayerBody", Model::LoadObjFile("Model/PlayerModel/Body/", "Body.obj")); // プレイヤーの体
	modelManager_->AddModel("PlayerEye", Model::LoadObjFile("Model/PlayerModel/Eye/", "Eye.obj")); // プレイヤーの瞳
	modelManager_->AddModel("PlayerHelmet", Model::LoadObjFile("Model/PlayerModel/Helmet/", "Helmet.obj")); // プレイヤーのヘルメット
	modelManager_->AddModel("PlayerLing", Model::LoadObjFile("Model/PlayerModel/Ling/", "Ling.obj")); // プレイヤーの輪っか
	modelManager_->AddModel("PlayerArm_L", Model::LoadObjFile("Model/PlayerModel/CharaArm/", "Arm_L.obj")); // プレイヤーの左腕
	modelManager_->AddModel("PlayerArm_R", Model::LoadObjFile("Model/PlayerModel/CharaArm/", "Arm_R.obj")); // プレイヤーの右腕
	modelManager_->AddModel("PlayerFoot_L", Model::LoadObjFile("Model/PlayerModel/Foot/", "Foot_L.obj")); // プレイヤーの左足
	modelManager_->AddModel("PlayerFoot_R", Model::LoadObjFile("Model/PlayerModel/Foot/", "Foot_R.obj")); // プレイヤーの右足
	modelManager_->AddModel("SkyDome", Model::LoadObjFile("Model/SkyDome/", "SkyDome.obj"))->materialMap_.begin()->second->materialBuff_->emissive = { 1.0f, 1.0f, 1.0f,1.0f };
	modelManager_->AddModel("Flag", Model::LoadObjFile("Model/Flag/", "Flag.obj")); // 旗

	modelManager_->AddModel("StarItem", Model::LoadObjFile("Model/Star/", "Star.obj"));

	/// UI関係読み込み

	// ステージ選択画面
	TextureManager::Load("UI/TD2_3week_2/StageSelect/StageSelectUI.png");		   // ステージ選択画面
	TextureManager::Load("UI/TD2_3week_2/StageSelect/StageSelectCursorLeft.png");  // ステージ選択画面選択矢印_左
	TextureManager::Load("UI/TD2_3week_2/StageSelect/StageSelectCursorRight.png"); // ステージ選択画面_右
	// ゲーム画面
	TextureManager::Load("UI/TD2_3week_2/InGame/ingameStopUI.png");		// 一時停止画面UI
	TextureManager::Load("UI/TD2_3week_2/InGame/ingameStopCursor.png"); // 停止カーソル

	// モデルロードが終わり次第シーンを離れる
	sceneManager_->ChangeScene(std::make_unique<TitleScene>(), 0.1f);
}

void LoadScene::OnExit()
{

}

void LoadScene::Update()
{

}

void LoadScene::Draw()
{
	// このシーンでは描画を行わない
}
