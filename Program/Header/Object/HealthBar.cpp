#include "HealthBar.h"

HealthBar::HealthBar() :backGround_(Sprite::Generate()), gauge_(Sprite::Generate()), decrementGauge_(Sprite::Generate()) {
}

void HealthBar::Init() {

	const uint32_t barTexture = TextureManager::Load("UI/BossBarFlame.png");
	const auto &texDesc = TextureManager::GetInstance()->GetResourceDesc(barTexture);
	const Vector2 textureSize = { static_cast<float>(texDesc.Width), static_cast<float>(texDesc.Height) };


	backGround_->SetTextureHaundle(barTexture);
	backGround_->SetColor(0xFFFFFFFF);
	backGround_->SetScale(vBarScale_);
	backGround_->SetPivot({ 0.5f,0.5f });
	backGround_->SetPosition(vBarCentor_);

	barFlameSize_ = { vBarScale_->x * (vBarFlame_->x / textureSize.x), vBarScale_->y * (vBarFlame_->y / textureSize.y) };

	gauge_->SetColor(0x00FF00FF);
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

void HealthBar::SetPercent(const float t) {
	Vector2 barSize = *vBarScale_ - barFlameSize_ * 2.f;
	barSize.x *= t;
	gauge_->SetScale(barSize);
}

void HealthBar::SetCentor(const Vector2 pos)
{
	vBarCentor_ = pos;
	backGround_->SetPosition(vBarCentor_);
	gauge_->SetPosition(*vBarCentor_ - Vector2{ vBarScale_->x * 0.5f - barFlameSize_.x,0.f });
}

void HealthBar::SetScale(const Vector2 radius)
{
	const uint32_t barTexture = backGround_->GetTexHandle();
	const auto &texDesc = TextureManager::GetInstance()->GetResourceDesc(barTexture);
	const Vector2 textureSize = { static_cast<float>(texDesc.Width), static_cast<float>(texDesc.Height) };

	vBarScale_ = radius;
	backGround_->SetScale(vBarScale_);

	barFlameSize_ = { vBarScale_->x * (vBarFlame_->x / textureSize.x), vBarScale_->y * (vBarFlame_->y / textureSize.y) };

	gauge_->SetScale(*vBarScale_ - barFlameSize_ * 2.f);
	gauge_->SetPosition(*vBarCentor_ - Vector2{ vBarScale_->x * 0.5f - barFlameSize_.x,0.f });
}
