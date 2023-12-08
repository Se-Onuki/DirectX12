#include "Fade.h"

#include "../../Engine/DirectBase/Base/WinApp.h"
#include "../../Utils/SoLib/SoLib_Lerp.h"

const Vector4 Fade::kFadeColor_ = Vector4{ 0xed,0x7b,0x68,0x00 };

void Fade::StaticInit() {
	auto *const instance = GetInstance();
	instance->timer_ = std::make_unique<SoLib::DeltaTimer>();
	instance->timer_->Clear();

	auto *const winApp = WinApp::GetInstance();

	instance->sprite_.reset(Sprite::Create());
	instance->sprite_->SetScale({ static_cast<float>(winApp->kWindowWidth), static_cast<float>(winApp->kWindowHeight) });

}

void Fade::SetState(const Vector2 &pos, const Vector4 &color) {
	sprite_->SetPosition(pos);
	sprite_->SetColor(color);
}

void Fade::Start(const Vector2 &targetPos, const Vector4 &targetColor, float goalTime) {

	// 目的地と、現在地を設定
	targetPos_ = targetPos;
	originalPos_ = sprite_->GetTransform().translate.ToVec2();

	// 目的colorと、現在colorを設定
	targetColor_ = targetColor;
	originalColor_ = sprite_->GetColor();

	// タイマーの実行
	timer_->Start(goalTime);
}

void Fade::Update(float deltaTime) {
	if (timer_->Update(deltaTime)) {	// 更新中だけ走る
		const float t = easeFunc_(timer_->GetProgress());

		// 画像位置を変更
		sprite_->SetPosition(Vector2{
			std::lerp(originalPos_.x, targetPos_.x, t),
			std::lerp(originalPos_.y, targetPos_.y, t)
			});

		// colorを変更
		sprite_->SetColor(SoLib::Lerp(originalColor_, targetColor_, t));

		if (timer_->IsFinish()) {	// ゴール到達時にのみ走る
			sprite_->SetPosition(targetPos_);			// 画像位置を設定
			sprite_->SetColor(targetColor_);	// colorを変更
			timer_->Clear();	// タイマーの初期化
		}
	}
}

void Fade::Draw() {
	sprite_->Draw();
}
