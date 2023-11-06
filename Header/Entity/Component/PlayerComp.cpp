#include "PlayerComp.h"
#include "Rigidbody.h"
#include "../../Object/LevelElementManager.h"

void PlayerComp::Init() {
	input_ = Input::GetInstance();
	collider_.min = -Vector3::one;
	collider_.max = Vector3::one;
	//auto *const rigidbody = object_->GetComponent<Rigidbody>();
	//rigidbody->ApplyInstantForce(Vector3{ 0.f,1000000.f,0.f });
}

void PlayerComp::Update() {


	static const auto *const keyBoard = input_->GetDirectInput();
	static auto *const levelManager = LevelElementManager::GetInstance();

	Vector3 inputVec{};
	auto *const rigidbody = object_->GetComponent<Rigidbody>();
	if (keyBoard) {
		if (keyBoard->IsTrigger(DIK_SPACE)) {
			rigidbody->ApplyInstantForce(Vector3{ 0.f,8.f,0.f });
		}


		if (keyBoard->IsPress(DIK_W)) {
			inputVec += Vector3::front;
		}
		if (keyBoard->IsPress(DIK_S)) {
			inputVec -= Vector3::front;
		}

		if (keyBoard->IsPress(DIK_A)) {
			inputVec -= Vector3::right;
		}
		if (keyBoard->IsPress(DIK_D)) {
			inputVec += Vector3::right;
		}

		rigidbody->ApplyContinuousForce(inputVec * vMoveSpeed);
	}
	rigidbody->ApplyContinuousForce(Vector3{ 0.f,-9.8f,0.f });


	LineBase moveLine{ .origin = rigidbody->GetBeforePos(), .diff = transform_->translate - rigidbody->GetBeforePos() };

	while (true) {

		float t = 1.f;
		Vector3 hitSurfaceNormal{};
		if (moveLine.diff.LengthSQ() > 0.f) {
			const AABB beforeCollider = collider_.AddPos(moveLine.origin);
			const AABB extendCollider = beforeCollider.Extend(moveLine.diff);

			const auto &vertexPos = beforeCollider.GetVertex();

			std::array<LineBase, 8u> vertexLine;
			for (uint32_t i = 0u; i < vertexLine.size(); ++i) {
				vertexLine[i].origin = vertexPos[i];
				vertexLine[i].diff = moveLine.diff;
			}


			for (const auto &[key, collider] : levelManager->blockCollider_) {
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
		}
		if (t < 1.f) {

			moveLine.origin = moveLine.GetProgress(t);
			Vector3 wallShear = (moveLine.diff - (moveLine.diff * hitSurfaceNormal) * hitSurfaceNormal) * (1.f - t);
			moveLine.diff = wallShear;
			Vector3 velocity = rigidbody->GetVelocity();
			for (uint32_t i = 0u; i < 3u; ++i) {
				velocity.data()[i] *= 1.f - std::abs(hitSurfaceNormal.data()[i]);
			}
			rigidbody->SetVelocity(velocity);
		}
		else {
			moveLine.origin = moveLine.GetEnd();
			break;
		}

	}


	transform_->translate = moveLine.origin;
}

void PlayerComp::Draw([[maybe_unused]] const Camera3D &camera) const {

}
