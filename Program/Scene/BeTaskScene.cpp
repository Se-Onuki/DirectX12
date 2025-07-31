#include "BeTaskScene.h"
#include "../Engine/Utils/Network/WindowsSocket/WindowsSocket.h"
#include <curl/curl.h>
#include "../Engine/Utils/Network/Curl/Curl.h"

#include "../Engine/Utils/SoLib/NewImGui.h"

BeTaskScene::BeTaskScene()
{
	// 入力インスタンスの取得
	input_ = SolEngine::Input::GetInstance();
	// 音インスタンスの取得
	audio_ = SolEngine::Audio::GetInstance();
	// カメラマネージャーの取得
	cameraManager_ = SolEngine::CameraManager::GetInstance();

	curl_global_init(CURL_GLOBAL_ALL);
}

BeTaskScene::~BeTaskScene()
{
	curl_global_cleanup();
}

void BeTaskScene::OnEnter()
{
	// タイマーの場所を設定
	SetTimerPositon();

	scoreText_ = SolEngine::NumberText::Generate(TextureManager::Load("UI/Number.png"), 4);
	scoreText_->SetPosition(Vector2{ WinApp::kWindowWidth / 2.f, 96.f * 2 });

	for (float i = 0; auto &num : topScoreText_) {
		num = SolEngine::NumberText::Generate(TextureManager::Load("UI/Number.png"), 4);
		num->SetPosition(Vector2{ WinApp::kWindowWidth / 2.f, 96.f * i++ + 96.f * 3 });
	}
}

void BeTaskScene::OnExit()
{
}

void BeTaskScene::Update()
{
	static std::string name;
	static std::string password;
	static std::string loginResult;

	ImGui::Begin("Curl操作");
	SoLib::ImGuiWidget("name", &name);
	SoLib::ImGuiWidget("password", &password);
	if (ImGui::Button("Login")) {
		// ログイン処理を非同期で行う
		loginResult = LoginAsync(name, password).get();
	}
	SoLib::NewImGui::ImGuiWidget(loginResult);
	ImGui::End();

	// タイマーが動作してたら更新処理を行う
	if (state_ == TaskState::kStart) {
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
		if (state_ == TaskState::kStart) {
			// タイマーを停止
			state_ = TaskState::kStop;

			scoreText_->SetText(TimeToScore(timeDuration_), true);

		}
		// もし開始していなかったら
		else if (state_ == TaskState::kStop or state_ == TaskState::kSendScore) {
			// タイマーを開始
			state_ = TaskState::kStart;
			// 高精度クロックの現在時刻を取得
			startTime_ = std::chrono::high_resolution_clock::now();
		}

	}

	if (state_ == TaskState::kStop) {
		// スコアを送信する
		if (input_->GetDirectInput()->IsTrigger(DIK_RETURN)) {
			state_ = TaskState::kSendScore;
			// スコアを送信
			PostScoreAsync(TimeToScore(timeDuration_)).get();
			// スコアの取得
			GetTopScore();
		}
	}

}

void BeTaskScene::Draw()
{
	DirectXCommon *const dxCommon = DirectXCommon::GetInstance();
	ID3D12GraphicsCommandList *const commandList = dxCommon->GetCommandList();

	Sprite::StartDraw(commandList);

	// 規定された時間以下なら表示する
	if (timeDuration_.count() < drawTimeCount_ * 1000 or state_ != TaskState::kStart) {
		// スプライトの描画
		DrawTimerText();
	}
	if (state_ != TaskState::kStart) {
		// スコアの表示
		scoreText_->Draw();

		for (const auto &num : topScoreText_) {
			num->Draw();
		}

	}

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

uint32_t BeTaskScene::TimeToScore(const std::chrono::milliseconds &time) const
{
	// 十秒を超過していたら0点
	if (time.count() >= (10 * 1000ll)) {
		return 0;
	}

	// 10秒からの差を計算
	// 10秒 = 10000ミリ秒
	const int32_t diff = std::abs(10000 - static_cast<int32_t>(time.count()));

	// 10秒から離れるごとに1000点から得点が減る｡0.001秒で1点減る｡
	int32_t score = 1000 - diff;
	// もしマイナスになっていたら0点
	if (score < 0) {
		score = 0;
	}

	return static_cast<uint32_t>(score);
}

void BeTaskScene::SendScore()
{
	PostScoreAsync(TimeToScore(timeDuration_)).get();
}

void BeTaskScene::GetTopScore()
{

	auto getRes = GetAllScoreAsync().get();

	if (getRes != "CURL初期化エラー") {
		try {
			nlohmann::json jsonData = nlohmann::json::parse(getRes);
			for (uint32_t i = 0; i < topScoreText_.size(); i++) {
				if (i < jsonData.size()) {
					topScoreText_[i]->SetText(jsonData[i]["score"].get<int32_t>(), true);
				}
				else {
					topScoreText_[i]->SetText(0, true);
				}
			}
		}
		catch ([[maybe_unused]] const nlohmann::json::parse_error &e) {

			return;
		}
	}



}

std::future<std::string> BeTaskScene::PostScoreAsync(int32_t score)
{
	{
		return std::async(std::launch::async, [score]() ->std::string {
			SoLib::Curl curl;
			/*CURL *curl = curl_easy_init();*/
			curl.Init();

			if (not curl) {
				return "CURL初期化エラー";
			}
			nlohmann::json body = nlohmann::json::object();
			body["score"] = score;

			std::string bodyStr = body.dump();

			struct curl_slist *headers = nullptr;
			headers = curl_slist_append(headers, "Content-Type: application/json");

			std::string response{};

			curl.SetOption(CURLOPT_URL, "https://swgame-se-onuki-onuki-seiyas-projects.vercel.app/scores");
			curl.SetOption(CURLOPT_HTTPHEADER, headers);
			curl.SetOption(CURLOPT_POST, 1L);
			curl.SetOption(CURLOPT_POSTFIELDS, bodyStr.c_str());
			curl.SetOption(CURLOPT_WRITEFUNCTION, SoLib::Network::WriteCallback);
			curl.SetOption(CURLOPT_WRITEDATA, &response);

			CURLcode res = curl.Perform();

			long httpCode = 0;

			curl.GetInfo(CURLINFO_RESPONSE_CODE, &httpCode);

			curl_slist_free_all(headers);
			curl.CleanUp();

			if (res != CURLE_OK) {
				return "送信エラー: " + std::string(curl_easy_strerror(res));
			}

			std::stringstream ss;
			ss << "HTTP " << httpCode << ": " << response;
			return ss.str();


			}
		);
	}
}

std::future<std::string> BeTaskScene::GetAllScoreAsync()
{
	return std::async(std::launch::async, []()->std::string {
		CURL *curl = curl_easy_init();
		if (not curl) {
			return "CURL初期化エラー";
		}

		// 応答を格納する文字列
		std::string response{};
		// CURLのオプションを設定
		curl_easy_setopt(curl, CURLOPT_URL, "https://swgame-se-onuki-onuki-seiyas-projects.vercel.app/scores");
		curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, SoLib::Network::WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

		// CURLを実行して応答を取得
		CURLcode res = curl_easy_perform(curl);
		// curlのクリーンアップ
		curl_easy_cleanup(curl);

		// 応答コードがCURLE_OKでない場合はエラーメッセージを返す
		if (res != CURLE_OK) {
			return "取得エラー: " + std::string(curl_easy_strerror(res));
		}
		return response;
		}
	);
}

std::future<std::string> BeTaskScene::LoginAsync(const std::string &name, const std::string &password)
{
	nlohmann::json body = nlohmann::json::object();
	body["name"] = name;
	body["password"] = password;
	const std::string bodyStr = body.dump();
	return std::async(std::launch::async, [bodyStr]()->std::string {
		SoLib::Curl curl;
		curl.Init();

		struct curl_slist *headers = nullptr;
		headers = curl_slist_append(headers, "Content-Type: application/json");

		std::string response{};

		curl.SetOption(CURLOPT_URL, "https://swgame-se-onuki-onuki-seiyas-projects.vercel.app/login");
		curl.SetOption(CURLOPT_HTTPHEADER, headers);
		curl.SetOption(CURLOPT_POST, 1L);
		curl.SetOption(CURLOPT_POSTFIELDS, bodyStr.c_str());
		curl.SetOption(CURLOPT_WRITEFUNCTION, SoLib::Network::WriteCallback);
		curl.SetOption(CURLOPT_WRITEDATA, &response);

		CURLcode res = curl.Perform();

		long httpCode = 0;

		curl.GetInfo(CURLINFO_RESPONSE_CODE, &httpCode);

		curl_slist_free_all(headers);
		curl.CleanUp();

		if (res != CURLE_OK) {
			return "ログインエラー: " + std::string(curl_easy_strerror(res));
		}

		return response;

		}
	);
}
