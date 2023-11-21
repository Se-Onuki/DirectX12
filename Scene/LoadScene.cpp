#include "LoadScene.h"
#include "TitleScene.h"

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
	textureManager_ = TextureManager::GetInstance(); // テクスチャ
	modelManager_ = ModelManager::GetInstance();	 // モデル

	// モデルの読み込み
	modelManager_->CreateDefaultModel(); // デフォルトモデルの読み込み
	modelManager_->AddModel("Box", Model::LoadObjFile("", "box.obj"));
	modelManager_->AddModel("RedBox", Model::LoadObjFile("", "box.obj"))->materialMap_["Material"]->materialBuff_->color = Vector4{ 1.f,0.f,0.f,1.f };
	modelManager_->AddModel("GrassModel", Model::LoadObjFile("Model/Ground/", "Ground.obj"))->materialMap_["Material"]->texHandle_ = textureManager_->Load("Model/Ground/GrassTex.png");
	modelManager_->AddModel("DirtModel", Model::LoadObjFile("Model/Ground/", "Ground.obj"))->materialMap_["Material"]->texHandle_ = textureManager_->Load("Model/Ground/DustTex.png");

	modelManager_->AddModel("PlayerBody", Model::LoadObjFile("Model/PlayerModel/Body/", "Body.obj")); // プレイヤーの体
	modelManager_->AddModel("PlayerEye", Model::LoadObjFile("Model/PlayerModel/Eye/", "Eye.obj")); // プレイヤーの瞳
	modelManager_->AddModel("PlayerHelmet", Model::LoadObjFile("Model/PlayerModel/Helmet/", "Helmet.obj")); // プレイヤーのヘルメット
	modelManager_->AddModel("PlayerLing", Model::LoadObjFile("Model/PlayerModel/Ling/", "Ling.obj")); // プレイヤーの輪っか
	modelManager_->AddModel("PlayerArm_L", Model::LoadObjFile("Model/PlayerModel/CharaArm/", "Arm_L.obj")); // プレイヤーの左腕
	modelManager_->AddModel("PlayerArm_R", Model::LoadObjFile("Model/PlayerModel/CharaArm/", "Arm_R.obj")); // プレイヤーの右腕
	modelManager_->AddModel("PlayerFoot_L", Model::LoadObjFile("Model/PlayerModel/Foot/", "Foot_L.obj")); // プレイヤーの左足
	modelManager_->AddModel("PlayerFoot_R", Model::LoadObjFile("Model/PlayerModel/Foot/", "Foot_R.obj")); // プレイヤーの右足
}

void LoadScene::OnExit()
{

}

void LoadScene::Update()
{
	// モデルロードが終わり次第シーンを離れる
	sceneManager_->ChangeScene(std::make_unique<TitleScene>(), 0.1f);
}

void LoadScene::Draw()
{
	// このシーンでは描画を行わない
}
