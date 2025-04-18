#include "ResultScene.h"
#include "../Engine/LevelEditor/LevelData.h"
#include "../Engine/LevelEditor/LevelImporter.h"
#include "../Engine/ECS/System/NewSystems.h"

ResultScene::ResultScene()
{
	input_ = SolEngine::Input::GetInstance();
	audio_ = SolEngine::Audio::GetInstance();
	fade_ = Fade::GetInstance();
}

ResultScene::~ResultScene()
{
}

void ResultScene::OnEnter()
{
	// ライトの生成
	light_ = DirectionLight::Generate();
	blockHandleRender_ = ModelHandleListManager::GetInstance();
	blockHandleRender_->Init(1024u);
	fade_->Start(Vector2{}, 0x00000000, 1.f);

	camera_.Init();

	camera_.translation_.y = 5.f;

	SolEngine::ResourceObjectManager<SolEngine::LevelData> *const levelDataManager = SolEngine::ResourceObjectManager<SolEngine::LevelData>::GetInstance();

	auto levelData = levelDataManager->Load({ .fileName_ = "check.json" });

	SolEngine::LevelImporter levelImporter;
	levelImporter.Import(levelData, &world_);

	levelDataManager->Destory(levelData);

	systemExecuter_.AddSystem<ECS::System::Par::CalcEulerTransMatrix>();
	systemExecuter_.AddSystem<ECS::System::Par::CalcTransMatrix>();
	systemExecuter_.AddSystem<ECS::System::Par::ModelDrawer>();
}

void ResultScene::OnExit()
{
}

void ResultScene::Update()
{

	[[maybe_unused]] const float deltaTime = std::clamp(ImGui::GetIO().DeltaTime, 0.f, 0.1f);

	camera_.rotation_ *= SoLib::MakeQuaternion(SoLib::Euler{ 0.f, 11.25_deg * deltaTime, 0.f });
	camera_.UpdateMatrix();

	blockHandleRender_->clear();

	if (input_->GetDirectInput()->IsTrigger(DIK_SPACE) or input_->GetXInput()->IsTrigger(SolEngine::KeyCode::A)) {
		sceneManager_->ChangeScene("TitleScene", 0.5f);
		fade_->Start(Vector2{}, 0x000000FF, 0.5f);
	}

	systemExecuter_.Execute(&world_, deltaTime);
}

void ResultScene::Draw()
{
	DirectXCommon *const dxCommon = DirectXCommon::GetInstance();
	ID3D12GraphicsCommandList *const commandList = dxCommon->GetCommandList();


#pragma region モデル描画

	Model::StartDraw(commandList);
	Model::SetPipelineType(Model::PipelineType::kShadowParticle);

	light_->SetLight(commandList);

	blockHandleRender_->Draw(camera_);

	Model::EndDraw();

#pragma endregion


	Sprite::StartDraw(commandList);

	// スプライトの描画
	fade_->Draw();

	Sprite::EndDraw();

}

void ResultScene::SetGameScore(const GameScore &gameScore)
{
	gameScore_ = gameScore;
}
