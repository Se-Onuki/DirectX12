/// @file LevelUP.cpp
/// @brief レベルアップのUI
/// @author ONUKI seiya
#include "LevelUP.h"
#include "../../../Engine/DirectBase/Input/Input.h"
#include <ranges>

void LevelUP::Init(int32_t count)
{
	// 背景の画像
	backGround_ = Sprite::Generate();

	// ボタンの数
	targetCount_ = count;

	// メモリ確保
	button_.resize(count);

	// データの構築
	std::generate(button_.begin(), button_.end(), ButtonUI::Generate);

	// サイズを合わせる
	buttonUIGroup_.buttonUIList_.resize(count);

	// ランダム配置
	RandomSet();
}

void LevelUP::SetWindow(Vector2 center, Vector2 scale, float distance)
{
	windowCenter_ = center;
	buttonScale_ = scale;
	distance_ = distance;
}

void LevelUP::Open(int32_t target)
{
	// 開いてたら終わる
	if (isOpen_) { return; }

	isSelect_ = false;

	// メニューを開く
	isOpen_ = true;
	// タイマーを開始する
	timer_.Start();

	// ランダム配置
	RandomSet();
	// カーソルをあわせる
	Target(target);
}

void LevelUP::Close()
{
	// 閉じてたら終わる
	if (not isOpen_) { return; }

	// メニューを閉じる
	isOpen_ = false;
	// タイマーを開始する
	timer_.Start();
}

void LevelUP::InputFunc()
{

	if (this->OpenProgress() <= 0.f) { return; }

	// 入力処理
	const SolEngine::Input *const pInput = SolEngine::Input::GetInstance();

	const SolEngine::XInput *const pXInput = pInput->GetXInput();
	const SolEngine::DirectInput *const pDInput = pInput->GetDirectInput();

	// 左入力
	const bool isLeft = pXInput->IsTrigger(SolEngine::KeyCode::DPAD_LEFT) || pXInput->GetState()->stickL_.x < 0.f || pDInput->IsTrigger(DIK_A) || pDInput->IsTrigger(DIK_LEFT);
	// 右入力
	const bool isRight = pXInput->IsTrigger(SolEngine::KeyCode::DPAD_RIGHT) || pXInput->GetState()->stickL_.x > 0.f || pDInput->IsTrigger(DIK_D) || pDInput->IsTrigger(DIK_RIGHT);

	int32_t move{};

	if (isLeft) { move--; }
	if (isRight) { move++; }

	// 移動したか
	const bool isMoved = (isLeft or isRight);

	// 移動してたら
	if (isMoved) {
		Target(target_ + move);	 // 移動した先を入力する
	}

	// 決定したか
	const bool isPush = pXInput->IsTrigger(SolEngine::KeyCode::A) or pDInput->IsTrigger(DIK_RETURN) or pDInput->IsTrigger(DIK_SPACE);

	// 決定していたら
	if (isPush) {
		// 決定フラグを立てる
		isSelect_ = true;

		// 実行する
		buttonPicker_.Execute();
		Close();
	}
}

void LevelUP::Update(const float deltaTime)
{

	// タイマーの更新
	timer_.Update(deltaTime);

	// ボタンの数
	const size_t buttonCount = buttonUIGroup_.buttonUIList_.size();
	const float halfSize = (buttonScale_.x + distance_) / 2 * buttonCount;

	// ボタンの選択の更新
	buttonPicker_.Update(deltaTime);

	buttonUIGroup_.SetStatus(
		[buttonCount, halfSize, this](ButtonUI *const button, const size_t index) {
			const float t = static_cast<float>(index) / buttonCount;
			button->sprite_->SetPosition(Vector2{ windowCenter_.x - (halfSize) * 0.5f - distance_ * 2.f + halfSize * 2 * t, windowCenter_.y });
			button->sprite_->SetScale(buttonScale_);
			// カラーを設定する
			SoLib::Color::RGB4 color = button->sprite_->GetColor();
			// 透明度をリンクする
			color.a = this->OpenProgress();
			// 色を返す
			button->sprite_->SetColor(color);
		}
	);
}

void LevelUP::Draw() const
{
	// 背景
	backGround_->Draw();

	// ボタンの描画
	for (int32_t i = 0; i < targetCount_ and i < buttonUIGroup_.buttonUIList_.size(); i++) {
		if (buttonUIGroup_.buttonUIList_[i]) { buttonUIGroup_.buttonUIList_[i]->Draw(); }
	}
}

void LevelUP::Target(int32_t target)
{
	// 移動したか
	const bool isMoved = (target >= 0 and target < targetCount_) and target != target_;	// 範囲内でなおかつ今と違うものである

	// 移動しているか､選択されてない場合
	if (isMoved or not buttonPicker_.IsPick()) {

		// ターゲットを指定する
		target_ = std::clamp(target, 0, targetCount_ - 1);

		// ボタンを指定する
		buttonPicker_.Pickup(buttonUIGroup_.buttonUIList_.at(target_));
	}
}

void LevelUP::RandomSet()
{
	std::vector<int32_t> indices(button_.size());
	auto iota = std::views::iota(0, static_cast<int32_t>(button_.size()));
	std::transform(iota.begin(), iota.end(), indices.begin(), [](const int32_t i)->int32_t { return i; });

	static std::mt19937 gen;
	static bool init;
	if (not init) {
		std::random_device rd;
		gen.seed(rd());
		init = true;
	}

	std::shuffle(indices.begin(), indices.end(), gen);

	std::transform(indices.begin(), indices.begin() + targetCount_, buttonUIGroup_.buttonUIList_.begin(), [this](const int32_t i)->ButtonUI *
		{
			return button_[i].get();
		}
	);
}

void LevelUP::push_back(std::unique_ptr<ButtonUI> &&button)
{
	button_.push_back(std::move(button));
}

std::unique_ptr<ButtonUI> ButtonUI::Generate()
{
	std::unique_ptr<ButtonUI> result = std::make_unique<ButtonUI>();
	result->sprite_ = Sprite::Generate();
	result->sprite_->SetPivot(Vector2::one * 0.5f);
	return std::move(result);
}

void ButtonUI::Init(const uint32_t texture, const std::function<void(void)> &func)
{
	sprite_->SetTextureHaundle(texture);

	execute_ = func;
}

void ButtonUI::Draw() const
{
	if (sprite_) {
		sprite_->Draw();
	}
}

void ButtonPickUp::Reset()
{
	pickUpTarget_ = nullptr;

	dropDownTarget_ = decltype(dropDownTarget_){};

	pickUpTimer_.Reset();
}

bool ButtonPickUp::ImGuiWidget()
{
	bool isChanged = false;

#ifdef USE_IMGUI

	isChanged |= SoLib::ImGuiWidget("PickUpEase", &pickUpEase_);
	isChanged |= SoLib::ImGuiWidget("PickUpTimer", &pickUpTimer_);

	isChanged |= SoLib::ImGuiWidget("DropDownEase", &dropDownEase_);
	isChanged |= SoLib::ImGuiWidget("DropDownTime", &dropDownTime_);

#endif // USE_IMGUI

	return isChanged;
}

void ButtonPickUp::Pickup(ButtonUI *target)
{
	// すでに選んでいる物があるなら
	if (pickUpTarget_) {

		// 末尾にデータがあるなら
		if (dropDownTarget_.back().first) {
			// 末尾のボタン
			auto *back = dropDownTarget_.back().first;
			// 終了時のデータを転送
			back->sprite_->SetColor(back->normalColor_);
		}

		// データを回す
		for (auto itr = dropDownTarget_.rbegin(); itr != dropDownTarget_.rend(); itr++) {
			auto next = std::next(itr);
			// 次のデータが末尾なら終わる
			if (next == dropDownTarget_.rend()) { break; }

			// 次のデータを自分のところに持ってくる
			*itr = *next;
		}

		// 先頭に今選択してるのを持ってくる
		dropDownTarget_.front() = { pickUpTarget_,0.f };
	}

	// ターゲットを設定する
	pickUpTarget_ = target;
	// タイマーを開始
	pickUpTimer_.Start();
}

void ButtonPickUp::Update(const float deltaTime)
{
	pickUpTimer_.Update(deltaTime);
	if (pickUpTarget_) {
		// ボタンのデータを書き換える
		pickUpTarget_->sprite_->SetColor(SoLib::Lerp(pickUpTarget_->normalColor_, pickUpTarget_->pickUpColor_, dropDownEase_(pickUpTimer_.GetProgress())));
	}

	std::for_each(dropDownTarget_.begin(), dropDownTarget_.end(), [this, deltaTime](std::pair<ButtonUI *, float> &item)
		{
			// 時間内で､対象が存在するなら
			if (item.second < dropDownTime_ and item.first) {
				// 時間を加算する
				item.second = std::clamp(item.second + deltaTime, 0.f, dropDownTime_); // 終端まで行ったら終端に合わせる
				// 時間に合わせてイージングを行う
				item.first->sprite_->SetColor(SoLib::Lerp(item.first->pickUpColor_, item.first->normalColor_, dropDownEase_(item.second / dropDownTime_)));
			}

			// 範囲外に行ったら
			if (item.second >= dropDownTime_) {
				item.first = nullptr;
			}
		}
	);

}

void ButtonPickUp::Execute() const
{
	// 選択対象が存在するなら
	if (pickUpTarget_) {
		// なおかつ関数が指定されているなら
		if (pickUpTarget_->execute_) {
			// それを実行
			pickUpTarget_->execute_();
		}
	}
}

void ButtonUIGroup::SetStatus(const std::function<void(ButtonUI *, size_t index)> &statusSetter) const
{
	for (size_t i = 0; i < buttonUIList_.size(); i++) {
		statusSetter(buttonUIList_[i], i);
	}
}
