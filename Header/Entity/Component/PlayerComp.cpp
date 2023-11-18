#include "PlayerComp.h"
#include "Rigidbody.h"
#include "../../Object/LevelElementManager.h"
#include "../../../Engine/DirectBase/File/GlobalVariables.h"
#include "ModelComp.h"
#include "PlayerAnimComp.h"

const std::string PlayerComp::groupName_ = "Player";

void PlayerComp::Init() {
	ApplyVariables(groupName_.c_str());
	input_ = Input::GetInstance();
	const Vector3 radius{ 0.75f,0.99f,0.75f };
	collider_.min = -radius;
	collider_.max = radius;

	backMaterial_.Create();
	backMaterial_.blendMode_ = Model::BlendMode::kBacker;
	backMaterial_.materialBuff_->color = Vector4{ 0.f,0.f,0.f,1.f };

	animationComp_ = object_->AddComponent<PlayerAnimComp>();

	AddVariable(groupName_.c_str());
}

void PlayerComp::Update() {
	ApplyVariables(groupName_.c_str());

	static const auto *const keyBoard = input_->GetDirectInput();
	static auto *const levelManager = LevelElementManager::GetInstance();

	Vector3 inputVec{};
	auto *const rigidbody = object_->GetComponent<Rigidbody>();
	if (keyBoard) {
		if (keyBoard->IsTrigger(DIK_SPACE)) {
			rigidbody->ApplyInstantForce(Vector3{ 0.f,vJumpPower,0.f });
		}

		if (keyBoard->IsTrigger(DIK_E)) {
			if (auto *const platform = levelManager->GetPlatform(registeredGroups_)) {
				platform->AddRotate(-90._deg);
			}
		}
		if (keyBoard->IsTrigger(DIK_Q)) {
			if (auto *const platform = levelManager->GetPlatform(registeredGroups_)) {
				platform->AddRotate(90._deg);
			}
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

		inputVec = inputVec.Nomalize();
	}
	// 入力強度を取得
	float movePower = 1.f;
	// もし入力が0でないなら強度に応じた値
	if (inputVec.LengthSQ() != 0.f) {
		movePower = inputVec.Length() / inputVec.Nomalize().Length();
	}

	// カメラの角度を元に計算
	inputVec = inputVec * pFollowCamera_->GetCamera().matView_.GetRotate().InverseRT();

	// カメラの上下方向を破棄
	inputVec.y = 0.f;
	inputVec = inputVec.Nomalize() * movePower;


	rigidbody->ApplyContinuousForce(inputVec * vMoveSpeed);
	rigidbody->ApplyContinuousForce(Vector3{ 0.f,-9.8f,0.f });


	LineBase moveLine{ .origin = rigidbody->GetBeforePos(), .diff = transform_->translate - rigidbody->GetBeforePos() };

	int32_t hitGroup = -1;
	while (true) {

		float t = 1.f;
		Vector3 hitSurfaceNormal{};
		if (moveLine.diff.LengthSQ() > 0.f) {
			const AABB beforeCollider = collider_.AddPos(moveLine.origin);
			const AABB extendCollider = beforeCollider.Extend(moveLine.diff);

			const auto &vertexPos = beforeCollider.GetVertex();

			LineBase centorLine;

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
									Vector3 minDiff = box.min - line.origin;
									Vector3 maxDiff = box.max - line.origin;
									if (normal * line.diff == -1.f) {

									}
									if (value < t) {
										t = value;
										hitSurfaceNormal = normal;
										if (normal * Vector3::up == 1.f) {
											hitGroup = static_cast<int32_t>(key);
										}
									}
								}
							}
						}
					}
				}
			}

		}
		registeredGroups_ = hitGroup;
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
	rigidbody->SetVelocity({ 0.f,rigidbody->GetVelocity().y,0.f });
}

void PlayerComp::Draw([[maybe_unused]] const Camera3D &camera) const {
	const auto *const modelComp = object_->GetComponent<ModelComp>();
	modelComp->Draw(camera, backMaterial_);
}

void PlayerComp::ImGuiWidget() {
	ImGui::Text("KeyGroup : %i", registeredGroups_);
}

void PlayerComp::ApplyVariables(const char *const groupName) {
	const GlobalVariables *const gVariable = GlobalVariables::GetInstance();
	const auto &cGroup = gVariable->GetGroup(groupName);

	cGroup >> vMoveSpeed;
	cGroup >> vJumpPower;
}

void PlayerComp::AddVariable(const char *const groupName) const {
	GlobalVariables *const gVariable = GlobalVariables::GetInstance();
	auto &group = gVariable->GetGroup(groupName);
	group << vMoveSpeed;
	group << vJumpPower;
}
