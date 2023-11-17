#include "CannonComp.h"
#include "../../../Utils/Math/Angle.h"
#include "Rigidbody.h"

void CannonComp::Init() {
}

void CannonComp::Update() {
}

void CannonComp::SetRider(Entity *const other) {
	// もし誰かが乗っているか、乗ったエンティティがnullptrであった場合は終了
	if (pRider_ && not other) {
		return;
	}

	pRider_ = other;
}

void CannonComp::Drop() {

	if (not pRider_) { return; }

	pRider_ = nullptr;
}

void CannonComp::Fire() {

	Vector3 moveVec = (Vector3::front * Matrix4x4::EulerRotate(Matrix4x4::EulerAngle::Yaw, angle_)) * vFirePower_;
	pRider_->GetComponent<Rigidbody>()->ApplyInstantForce(moveVec);

	this->Drop();

}

void CannonComp::AddRotate(float angle) {
	angle_ = Angle::Mod(angle_ + angle);
}
