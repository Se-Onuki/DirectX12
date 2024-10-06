#include "LevelUP.h"
#include "../../../Engine/DirectBase/Input/Input.h"

void LevelUP::Init(int32_t count)
{
	targetCount_ = count;

	button_.resize(count);

	//for (int32_t i = 0; i < count; i++) {
	//	auto &button = button_[i];
	//	button = std::unique_ptr<ButtonUI>();

	//	button->sprite_ = Sprite::Create();
	//}


}

void LevelUP::Start()
{
	// カーソルをあわせる
	Target(0);
}

void LevelUP::InputFunc()
{
	// 入力処理
	const Input *const pInput = Input::GetInstance();

	const XInput *const pXInput = pInput->GetXInput();
	const DirectInput *const pDInput = pInput->GetDirectInput();

	// 左入力
	const bool isLeft = pXInput->IsTrigger(KeyCode::DPAD_LEFT) || pXInput->GetState()->stickL_.x < 0.f || pDInput->IsTrigger(DIK_A) || pDInput->IsTrigger(DIK_LEFT);
	// 右入力
	const bool isRight = pXInput->IsTrigger(KeyCode::DPAD_RIGHT) || pXInput->GetState()->stickL_.x > 0.f || pDInput->IsTrigger(DIK_D) || pDInput->IsTrigger(DIK_RIGHT);

	int32_t move{};

	if (isLeft) { move--; }
	if (isRight) { move++; }

	// 移動したか
	const bool isMoved = (isLeft or isRight);

	// 移動してたら
	if (isMoved) {
		Target(target_ + move);	 // 移動した先を入力する
	}
}

void LevelUP::Update(const float deltaTime)
{
	// 時計の更新
	buttonPicker_.Update(deltaTime);
}

void LevelUP::Draw() const
{
	// 背景
	backGround_->Draw();

	// ボタンの描画
	for (int32_t i = 0; i < targetCount_ and i < button_.size(); i++) {
		if (button_[i]) { button_[i]->Draw(); }
	}
}

void LevelUP::Target(int32_t target)
{
	// 移動したか
	const bool isMoved = (target <= 0 and target > targetCount_) and target != target_;	// 範囲内でなおかつ今と違うものである

	// 移動しているか､選択されてない場合
	if (isMoved or not buttonPicker_.IsPick()) {

		// ターゲットを指定する
		target_ = std::clamp(target, 0, targetCount_ - 1);

		// ボタンを指定する
		buttonPicker_.Pickup(button_.at(target_).get());

	}
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
	isChanged |= pickUpTimer_.ImGuiWidget("PickUpTimer");

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
			back->sprite_->SetScale(back->normalTrans_.scale_);
			back->sprite_->SetRotate(back->normalTrans_.rotate_.Get());
			back->sprite_->SetPosition(back->normalTrans_.translate_);
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
		pickUpTarget_->sprite_->SetTransform(SoLib::Lerp(pickUpTarget_->pickUpTrans_, pickUpTarget_->normalTrans_, dropDownEase_(pickUpTimer_.GetProgress())));
	}

	std::for_each(dropDownTarget_.begin(), dropDownTarget_.end(), [this, deltaTime](std::pair<ButtonUI *, float> &item)
		{
			// 時間内で､対象が存在するなら
			if (item.second < dropDownTime_ and item.first) {
				// 時間を加算する
				item.second = std::clamp(item.second + deltaTime, 0.f, dropDownTime_); // 終端まで行ったら終端に合わせる
				// 時間に合わせてイージングを行う
				item.first->sprite_->SetTransform(SoLib::Lerp(item.first->pickUpTrans_, item.first->normalTrans_, dropDownEase_(item.second)));
			}
		}
	);
}