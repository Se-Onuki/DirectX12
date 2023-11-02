#include "PlayerComp.h"
#include "Rigidbody.h"
#include "../../Object/LevelElementManager.h"

void PlayerComp::Init() {
	input_ = Input::GetInstance();
	collider_.min = -Vector3::one;
	collider_.max = Vector3::one;
}

void PlayerComp::Update() {
	

	static const auto *const keyBoard = input_->GetDirectInput();
	static auto *const rigidbody = object_->GetComponent<Rigidbody>();
	if (keyBoard) {
		if (keyBoard->IsPress(DIK_SPACE)) {
			rigidbody->ApplyContinuousForce(Vector3{ 0.f,1.f,0.f });
		}
	}

	// 前の座標からどれだけ動いたか
	Vector3 diff = object_->transform_.translate - rigidbody->GetBeforePos();

	AABB beforeCollider = collider_.AddPos(rigidbody->GetBeforePos());

	const auto &vertexPos = beforeCollider.GetVertex();

	std::array<LineBase, 8u> vertexLine;
	for (uint32_t i = 0u; i < 8u; ++i) {
		vertexLine[i].lineType = LineBase::LineType::Segment;
		vertexLine[i].origin = vertexPos[i];
		vertexLine[i].diff = diff;
	}

	static auto *const levelManager = LevelElementManager::GetInstance();

	for (auto &[key, collider] : levelManager->blockCollider_) {
		for (auto &box : collider.GetCollider()) {

			// Todo : 現在地と終端地を含むSphereを使って簡易化する
			for (auto &line : vertexLine) {
				if (Collision::IsHit(box, line)) {

				}
			}
		}
	}
}

void PlayerComp::Draw([[maybe_unused]] const Camera3D &camera) const {

}
