#include "LevelUP.h"
#include "../../../Engine/DirectBase/Input/Input.h"

void LevelUP::Init()
{
}

void LevelUP::Start()
{
	timer_.Start();
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

	if (isLeft) { target_--; }
	if (isRight) { target_++; }
	target_ = std::clamp(target_, 0, targetCount_);

	// 入力がどっちか行われていたら
	if (isLeft or isRight) {


	}
}

void LevelUP::Update(const float deltaTime)
{
	timer_.Update(deltaTime);
}

void LevelUP::Draw() const
{
	backGround_->Draw();
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
