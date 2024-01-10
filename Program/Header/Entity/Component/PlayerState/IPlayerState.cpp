#include "IPlayerState.h"
#include "../PlayerComp.h"

IPlayerStateClass::IPlayerStateClass(PlayerComp *ptr) :pPlayer_(ptr), input_(Input::GetInstance()), pAnimation_(pPlayer_->object_->AddComponent<PlayerAnimComp>()) {}

void IPlayerStateClass::Init() {
}

void IPlayerStateClass::Update([[maybe_unused]] float deltaTime) {
}

void IPlayerStateClass::Draw([[maybe_unused]] const Camera3D &camera) const {
}
