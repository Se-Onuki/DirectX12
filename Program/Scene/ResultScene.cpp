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

	const std::string resultTexture = std::string("UI/") + (gameScore_.isClear_ ? "gameClear.png" : "gameOver.png");

	resultText_ = Sprite::Generate(TextureManager::Load(resultTexture));
	resultText_->SetPivot(Vector2::one * 0.5f);
	resultText_->SetPosition(Vector2{ WinApp::kWindowWidth * 0.5f, WinApp::kWindowHeight * 0.5f });
	resultText_->SetScale(Vector2{ 640.f,320.f }*2.f);

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

	for (auto &timeUI : gameTimerUI_) {

		timeUI = SolEngine::NumberText::Generate(TextureManager::Load("UI/Number.png"), 2u);
	}
	{
		// 時計の分秒への変換
		auto &&[m, s] = SoLib::Time::GetMoment(gameScore_.aliveTime_);
		gameTimerUI_[0]->SetText(m, true);
		gameTimerUI_[1]->SetText(s, true);
	}

	timerCoron_ = Sprite::Generate(TextureManager::Load("UI/Coron.png"));


	const Vector2 timerPos{ WinApp::kWindowWidth / 2.f ,96.f };
	SetTimerPos(timerPos);
}

void ResultScene::OnExit()
{
}

void ResultScene::Update()
{

#ifdef USE_IMGUI

	auto pos = timerCoron_->GetTransform().translate_.ToVec2();
	SoLib::ImGuiWidget("TimerPos", &pos);
	SetTimerPos(pos);

#endif // USE_IMGUI


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

	resultText_->Draw();

	//for (const auto &num : gameTimerUI_) {
	//	num->Draw();
	//}

	//timerCoron_->Draw();

	// スプライトの描画
	fade_->Draw();

	Sprite::EndDraw();

}

void ResultScene::SetGameScore(const GameScore &gameScore)
{
	gameScore_ = gameScore;
}

void ResultScene::SetTimerPos(Vector2 pos)
{
	static constexpr std::array<float, 2u> kIsPlus{ -1.f, 1.f };
	for (uint32_t i = 0; i < 2; i++) {
		auto &timeUI = gameTimerUI_[i];
		timeUI->SetPosition(pos + Vector2{ 48 * kIsPlus[i],0 });
		timeUI->SetPivot(Vector2{ static_cast<float>(1 - i), 0.f });
	}

	timerCoron_->SetPosition(pos);
	timerCoron_->SetPivot(Vector2::one * 0.5f);
	timerCoron_->SetScale(Vector2{ 18,96 });
}
