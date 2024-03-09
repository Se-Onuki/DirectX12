#include "HealthBar.h"

HealthBar::HealthBar() :backGround_(Sprite::Create()), gauge_(Sprite::Create()), decrementGauge_(Sprite::Create()) {
}

void HealthBar::Init() {

	const uint32_t barTexture = TextureManager::Load("UI/BossBarFlame.png");
	const auto &texDesc = TextureManager::GetInstance()->GetResourceDesc(barTexture);
	const Vector2 &textureSize = { static_cast<float>(texDesc.Width), static_cast<float>(texDesc.Height) };


	backGround_->SetTextureHaundle(barTexture);
	backGround_->SetColor(Vector4{ 1.f,1.f,1.f,1.f });
	backGround_->SetScale(vBarScale_);
	backGround_->SetPivot({ 0.5f,0.5f });
	backGround_->SetPosition(vBarCentor_);

	barFlameSize_ = { vBarScale_->x * (vBarFlame_->x / textureSize.x), vBarScale_->y * (vBarFlame_->y / textureSize.y) };

	gauge_->SetColor(Vector4{ 0.f,1.f,0.f,1.f });
	gauge_->SetScale(vBarScale_.GetItem() - barFlameSize_ * 2.f);
	gauge_->SetPivot({ 0.0f,0.5f });
	gauge_->SetPosition(vBarCentor_.GetItem() - Vector2{ vBarScale_->x * 0.5f - barFlameSize_.x,0.f });
}

void HealthBar::Update([[maybe_unused]] const float deltaTime) {
}

void HealthBar::Draw() {

	backGround_->Draw();
	gauge_->Draw();
}
