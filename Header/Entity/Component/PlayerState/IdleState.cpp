#include "IdleState.h"
#include "../../../../Engine/DirectBase/Input/Input.h"
#include "../Rigidbody.h"
#include "../PlayerComp.h"

void PlayerIdleState::Init() {

}

void PlayerIdleState::Update([[maybe_unused]] float deltaTime) {
	static auto input = Input::GetInstance();

	static const auto *const keyBoard = input->GetDirectInput();
	auto *const rigidbody = pPlayer_->object_->GetComponent<Rigidbody>();


	if (keyBoard->IsTrigger(DIK_P)) {
		rigidbody->ApplyInstantForce(Vector3{ 0.f,100.f,0.f });
	}
}

void PlayerIdleState::Draw([[maybe_unused]] const Camera3D &camera) const {

}
