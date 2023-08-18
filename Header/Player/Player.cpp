#include "Player.h"
#include "../../DirectBase/Input/Input.h"

void Player::Init(const std::unordered_map<std::string, Model *> &model) {
	BaseCharacter::Init(model);
	input_ = Input::GetInstance();
}

void Player::Update() {
	BaseCharacter::Update();
	Vector2 stickL = input_->GetXInput()->GetState()->stickL_;
	Vector3 inputMove = Vector3{ stickL.x, 0, stickL.y };
	transformOrigin_.translate += inputMove * 0.25f;

	transformOrigin_.ImGuiWidget();

	transformOrigin_.UpdateMatrix();
}

void Player::Draw(const ViewProjection &vp) const {
	for (auto &model : modelMap_) {
		model.second->Draw(transformOrigin_, vp);
	}
}
