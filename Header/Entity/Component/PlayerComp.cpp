#include "PlayerComp.h"
#include "Rigidbody.h"
#include "../../Object/LevelElementManager.h"

void PlayerComp::Init() {
	input_ = Input::GetInstance();
	collider_.min = -Vector3::one * 0.5f;
	collider_.max = Vector3::one * 0.5f;
	//auto *const rigidbody = object_->GetComponent<Rigidbody>();
	//rigidbody->ApplyInstantForce(Vector3{ 0.f,1000000.f,0.f });
}

void PlayerComp::Update() {


	static const auto *const keyBoard = input_->GetDirectInput();
	static auto *const levelManager = LevelElementManager::GetInstance();

	auto *const rigidbody = object_->GetComponent<Rigidbody>();
	if (keyBoard) {
		if (keyBoard->IsTrigger(DIK_SPACE)) {
			rigidbody->ApplyInstantForce(Vector3{ 0.f,10.f,0.f });
		}
	}
	rigidbody->ApplyContinuousForce(Vector3{ 0.f,-9.8f,0.f });

	// 前の座標からどれだけ動いたか
	Vector3 diff = transform_->translate - rigidbody->GetBeforePos();

	const AABB beforeCollider = collider_.AddPos(rigidbody->GetBeforePos());
	const AABB extendCollider = beforeCollider.Extend(diff);

	const auto &vertexPos = beforeCollider.GetVertex();

	std::array<LineBase, 8u> vertexLine;
	for (uint32_t i = 0u; i < 8u; ++i) {
		//	vertexLine[i].lineType = LineBase::LineType::Segment;
		vertexLine[i].origin = vertexPos[i];
		vertexLine[i].diff = diff;
	}

	float t = 1.f;

	Vector3 hitSurfaceNormal{};

	for (auto &[key, collider] : levelManager->blockCollider_) {
		for (auto &box : collider.GetCollider()) {
			// 拡張した箱が当たってたら詳細な判定
			if (Collision::IsHit(extendCollider, box)) {
				for (auto &line : vertexLine) {
					if (Collision::IsHit(box, line)) {

						float value = Collision::HitProgress(line, box);
						const Vector3 normal = box.GetNormal(line.GetProgress(value));
						if (normal * line.diff < 0.f) {
							if (value < t) {
								t = value;
								hitSurfaceNormal = normal;
							}
						}
					}
				}
			}
		}
	}

	transform_->translate = rigidbody->GetBeforePos() + diff * t;
	if (t < 1.f) {
		Vector3 velocity = rigidbody->GetVelocity();
		for (uint32_t i = 0u; i < 3u; ++i) {
			(&velocity.x)[i] *= 1.f - std::abs((&hitSurfaceNormal.x)[i]);
		}
		rigidbody->SetVelocity(velocity);
	}

}

void PlayerComp::Draw([[maybe_unused]] const Camera3D &camera) const {

}
