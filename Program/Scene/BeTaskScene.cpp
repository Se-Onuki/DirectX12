#include "BeTaskScene.h"

BeTaskScene::BeTaskScene()
{
	// 入力インスタンスの取得
	input_ = SolEngine::Input::GetInstance();
	// 音インスタンスの取得
	audio_ = SolEngine::Audio::GetInstance();
	// カメラマネージャーの取得
	cameraManager_ = SolEngine::CameraManager::GetInstance();
}

BeTaskScene::~BeTaskScene()
{

}

void BeTaskScene::OnEnter()
{
	// タイマーの場所を設定
	SetTimerPositon();


}

void BeTaskScene::OnExit()
{
}

void BeTaskScene::Update()
{

	// タイマーが動作してたら更新処理を行う
	if (isTimerStart_) {
		// 高精度クロックでの現在時刻
		auto now = std::chrono::high_resolution_clock::now();

		// 処理時間を計算
		timeDuration_ = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime_);
	}
	// テキストの更新
	TimeToText(timeDuration_);

	// 計測ボタンを押したら
	if (input_->GetDirectInput()->IsTrigger(DIK_SPACE)) {
		// もし開始していたら
		if (isTimerStart_) {
			// タイマーを停止
			isTimerStart_ = false;

		}
		// もし開始していなかったら
		else {
			// タイマーを開始
			isTimerStart_ = true;
			// 高精度クロックの現在時刻を取得
			startTime_ = std::chrono::high_resolution_clock::now();
		}

	}

}

void BeTaskScene::Draw()
{
	DirectXCommon *const dxCommon = DirectXCommon::GetInstance();
	ID3D12GraphicsCommandList *const commandList = dxCommon->GetCommandList();

	Sprite::StartDraw(commandList);

	// スプライトの描画
	DrawTimerText();

	Sprite::EndDraw();
}

void BeTaskScene::SetTimerPositon()
{

	static constexpr std::array<float, 2u> kIsPlus{ -1.f, 1.f };
	static constexpr std::array<uint32_t, 2u> kTextCount{ 2u, 4u };
	for (uint32_t i = 0; i < 2; i++) {
		auto &timeUI = number_[i];

		timeUI = SolEngine::NumberText::Generate(TextureManager::Load("UI/Number.png"), kTextCount[i]);
		timeUI->SetPosition(Vector2{ WinApp::kWindowWidth / 2.f + 48 * kIsPlus[i],96.f });
		timeUI->SetPivot(Vector2{ static_cast<float>(1 - i), 0.f });

	}
}

void BeTaskScene::TimeToText(const std::chrono::milliseconds &time)
{
	// 各数値に変換
	auto moment = SoLib::Time::GetMilliMoment(time);

	number_[0]->SetText(moment.first, true);
	number_[1]->SetText(moment.second, true);

}

void BeTaskScene::DrawTimerText() const
{
	for (const auto &num : number_) {
		num->Draw();
	}
}
