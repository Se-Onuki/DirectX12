#include "Systems.h"
#include "../World/World.hpp"
#include "../Entity/EntityManager.hpp"
#include "../../Header/Object/Particle/TestParticle.h"
#include "../../Engine/DirectBase/Input/Input.h"
#include "../../Engine/DirectBase/Render/CameraAnimations/CameraManager.h"

void ECS::System::CheckAliveTime::OnUpdate(::World *world, [[maybe_unused]] const float deltaTime) {

	for (const auto &[entity, aliveTime, lifeLimit, isAlive] : world->view<ECS::AliveTime, ECS::LifeLimit, ECS::IsAlive>()) {
		// もし寿命が定められていたら
		if (lifeLimit->lifeLimit_ >= 0.f) {
			// 寿命を超過していたら
			if (lifeLimit->lifeLimit_ < aliveTime->aliveTime_) {
				// 死ぬ
				isAlive->isAlive_ = false;
			}
		}
	}

}

void ECS::System::CheckHealthDie::OnUpdate(::World *world, [[maybe_unused]] const float deltaTime) {

	for (const auto &[entity, health, isAlive] : world->view<ECS::HealthComp, ECS::IsAlive>()) {
		// もし体力が0以下なら
		if (health->IsDead()) {

			// 死ぬ
			isAlive->isAlive_ = false;
		}
	}
}

void ECS::System::AddAliveTime::OnUpdate(::World *world, [[maybe_unused]] const float deltaTime) {


	for (const auto &[entity, aliveTime] : world->view<ECS::AliveTime>()) {
		// 生存時間を加算
		aliveTime->aliveTime_ += deltaTime;
	}

}

void ECS::System::AnimateUpdate::OnUpdate(::World *world, [[maybe_unused]] const float deltaTime) {

	for (const auto &[entity, animate] : world->view<ECS::AnimateParametor>()) {
		auto &timer = animate->timer_;

		// 動作パラメータを追加
		timer.Update(deltaTime);

		if (timer.IsActive() && timer.IsFinish()) {
			// 次のアニメーションの情報を格納

			// 仮実装なので、マジックナンバーで行う
			if (animate->animIndex_ == 0u) {
				animate->animIndex_ = 1u;
				timer.Start();
			}
			else {
				animate->animIndex_ = 0u;
			}


		}
	}

}

void ECS::System::ColorLerp::OnUpdate(::World *world, [[maybe_unused]] const float deltaTime) {


	for (const auto &[entity, aliveTime, lifelimit, colorLerp, color] : world->view<ECS::AliveTime, ECS::LifeLimit, ECS::ColorLarp, ECS::Color>()) {
		color->color_ = colorLerp->EaseColor(aliveTime->aliveTime_ / lifelimit->lifeLimit_);
	}
}

void ECS::System::AddGravity::OnUpdate(::World *world, [[maybe_unused]] const float deltaTime) {

	for (const auto &[entity, acceleration, gravity] : world->view<ECS::AccelerationComp, ECS::GravityComp>()) {

		acceleration->acceleration_ += gravity->gravity_ * deltaTime;

	}
}

void ECS::System::AirResistanceSystem::OnUpdate(::World *world, [[maybe_unused]] const float deltaTime) {

	for (const auto &[entity, acceleration, velocity, airResistance] : world->view<ECS::AccelerationComp, const ECS::VelocityComp, const ECS::AirResistance>()) {

		acceleration->acceleration_ -= velocity->velocity_ * airResistance->resistance;

	}
}


void ECS::System::MovePosition::OnUpdate(::World *world, const float deltaTime) {

	for (const auto &[entity, velocity, acceleration] : world->view<ECS::VelocityComp, ECS::AccelerationComp>()) {

		velocity->velocity_ += acceleration->acceleration_;
		acceleration->acceleration_ = {};

	}

	for (const auto &[entity, pos, velocity] : world->view<ECS::PositionComp, ECS::VelocityComp>()) {

		pos->position_ += velocity->velocity_ * deltaTime;

	}

}

void ECS::System::EnemyMove::OnUpdate(::World *world, [[maybe_unused]] const float deltaTime) {

	Vector3 playerPos{};
	const ECS::Entity *player = nullptr;
	for (const auto &[entity, playerTag, plPos] : world->view<const ECS::PlayerTag, const ECS::PositionComp>()) {
		playerPos = *plPos;
		player = entity;
	}
	// プレイヤが存在しない場合そこで終了
	if (not player) { return; }

	for (const auto &[entity, enemy, pos, rotate] : world->view<ECS::EnemyTag, ECS::PositionComp, ECS::QuaternionRotComp>()) {
		Vector3 direction = (playerPos - pos->position_).Nomalize() * (100.f * deltaTime * deltaTime);
		direction.y = 0.f;
		pos->position_ += direction;

		rotate->quateRot_ = Quaternion::LookAt(direction);

	}

}
void ECS::System::EnemyAttack::OnUpdate(::World *world, [[maybe_unused]] const float deltaTime) {

	Vector3 plPos{};
	ECS::AccelerationComp *plAccele = nullptr;
	ECS::HealthComp *plHealth = nullptr;
	Sphere plColl{};
	const ECS::Entity *player = nullptr;
	for (const auto &[entity, playerTag, playerPos, playerHealth, playerCollision, playerAcceleration] : world->view<const ECS::PlayerTag, const ECS::PositionComp, ECS::HealthComp, const ECS::CollisionComp, ECS::AccelerationComp>()) {
		// 体力のポインタを取得
		plHealth = playerHealth;
		// 加速度のポインタを取得
		plAccele = playerAcceleration;

		// 当たり判定を取得
		plColl = playerCollision->collision_;
		plColl.centor += *playerPos;

		// プレイヤの座標
		plPos = *playerPos;
		player = entity;
	}
	// プレイヤが存在しない場合そこで終了
	if (not player) { return; }

	for (const auto &[entity, enemy, pos, coll, attackPow, attackCT] : world->view<const ECS::EnemyTag, const ECS::PositionComp, const ECS::CollisionComp, ECS::AttackPower, ECS::AttackCooltime>()) {
		Sphere enColl = coll->collision_;
		enColl.centor += *pos;

		// クールタイムが終わっており、接触している場合
		if (not attackCT->cooltime_.IsActive() and Collision::IsHit(plColl, enColl)) {
			// プレイヤに加速度を加算
			plAccele->acceleration_ += (plPos - *pos) * 15.f;
			// 体力を減算
			plHealth->nowHealth_ -= attackPow->power_;

			// クールタイムを開始
			attackCT->cooltime_.Start();
		}

	}
}


void ECS::System::FallCollision::OnUpdate(::World *world, [[maybe_unused]] const float deltaTime) {

	for (const auto &[entity, collision, pos, velocity] : world->view<ECS::CollisionComp, ECS::PositionComp, ECS::VelocityComp>()) {
		const auto &[isLanding] = world->GetEntityManager()->GetComponent<ECS::IsLanding>(*entity);

		// 地面より座標が下なら
		if ((pos->position_.y + collision->collision_.centor.y - collision->collision_.radius) < ground_->hight_) {
			// 地面の上に当たり判定を上にする
			pos->position_.y = ground_->hight_ - collision->collision_.centor.y + collision->collision_.radius;
			// もし落下していたら
			if (velocity->velocity_.y < 0.f) {
				// 移動速度を0にする
				velocity->velocity_.y = 0.f;
			}

			if (isLanding) {
				isLanding->isLanding_ = true;
			}
		}
		else {

			if (isLanding) {
				isLanding->isLanding_ = false;
			}
		}
	}

}

void ECS::System::WeaponCollision::OnUpdate(::World *world, [[maybe_unused]] const float deltaTime) {
	static ParticleManager *const particleManager = ParticleManager::GetInstance();

	{
		std::list<const ECS::WeaponComp *> weaponList{};

		for (const auto &[entity, weapon] : world->view<const ECS::WeaponComp>()) {
			weaponList.push_back(weapon);
		}

		for (const auto &[entity, enemy, pos, collision, isAlive] : world->view<ECS::EnemyTag, ECS::PositionComp, ECS::CollisionComp, ECS::IsAlive>()) {

			Sphere sphere = collision->collision_;
			sphere.centor += *pos;

			for (const auto *weapon : weaponList) {
				if (Collision::IsHit(sphere, weapon->collision_)) {
					isAlive->isAlive_ = false;

					sound_.Play(false, 1.f);

					for (uint32_t i = 0u; i < 10u; i++) {
						auto particle = particleManager->AddParticle<TestParticle>(model_, sphere.centor);
						particle->SetAliveTime(Random::GetRandom<float>(0.5f, 1.5f));
						particle->acceleration_ = SoLib::Math::EulerToDirection(Vector3{ Random::GetRandom<float>(-Angle::hPI,Angle::hPI),Random::GetRandom<float>(-Angle::PI,Angle::PI),0.f }) * Random::GetRandom<float>(10.f, 20.f);
						particle->transform_.scale = Vector3::one * 5.f;
					}
				}
			}

		}
	}
}

void ECS::System::PlayerMove::OnUpdate(::World *world, [[maybe_unused]] const float deltaTime) {
	auto *inputManager = Input::GetInstance();

	for (const auto &[entity, pos, quateRot, acceleration, input, animate, isLanding] : world->view<ECS::PositionComp, ECS::QuaternionRotComp, ECS::AccelerationComp, ECS::InputFlagComp, ECS::AnimateParametor, ECS::IsLanding>()) {
		const auto *const camera = CameraManager::GetInstance()->GetCamera("FollowCamera");
		const Vector2 inputLs = inputManager->GetXInput()->GetState()->stickL_;
		const Vector3 input3d{ inputLs.x,0.f,inputLs.y };
		const Vector3 rotateInput = Quaternion::AnyAxisRotation(Vector3::up, camera->rotation_.y).RotateVector(input3d);
		pos->position_ += rotateInput * (500.f * deltaTime * deltaTime);

		if (input3d.LengthSQ()) {
			quateRot->quateRot_ = Quaternion::LookAt(rotateInput);
		}

		if (isLanding->isLanding_ && inputManager->GetXInput()->IsTrigger(KeyCode::A)) {
			acceleration->acceleration_.y += 10.f;
		}

		if (animate->animIndex_ == 0u && animate->timer_.IsFinish()) {
			if (inputManager->GetXInput()->GetState()->triggerR_ > 0.5f) {
				animate->timer_.Start(0.5f);
			}
		}
	}
}

void ECS::System::BillboardCalc::OnUpdate(::World *world, [[maybe_unused]] const float deltaTime) {
	Matrix4x4 billboardMat = CameraManager::GetInstance()->GetUseCamera()->matView_.GetRotate().InverseRT();

	for (const auto &[entity, scale, rotate, pos, mat, billboardRot] : world->view<ECS::ScaleComp, ECS::RotateComp, ECS::PositionComp, ECS::TransformMatComp, ECS::BillboardRotate>()) {

		*mat = Matrix4x4::Affine(*scale, rotate->rotate_, Vector3::zero);
		mat->transformMat_ *= billboardMat;
		*reinterpret_cast<Vector3 *>(mat->transformMat_.m[3].data()) = *pos;

	}

}

void ECS::System::BoneAnimationCalc::OnUpdate(::World *world, [[maybe_unused]] const float deltaTime) {
	for (const auto &[entity, scale, quate, pos, bone, animate] : world->view<ECS::ScaleComp, ECS::QuaternionRotComp, ECS::PositionComp, ECS::BoneTransformComp, ECS::AnimateParametor>()) {

		bone->boneTransform_[0] = { *scale, quate->quateRot_.Normalize(), *pos };

		// 頭のパラメータ
		{
			auto &head = bone->boneTransform_[boneModel_->GetIndex("Head")];
			head.translate_.y = 2.f;
			head.scale_ = Vector3::one * 0.5f;
		}

		// 体のパラメータ
		{
			auto &body = bone->boneTransform_[boneModel_->GetIndex("Body", 0)];

			body.scale_ = { 0.75f,1.f,0.75f };
		}

		// 剣のパラメータ
		{
			auto &swordModel = bone->boneTransform_[boneModel_->GetIndex("Sword", 0)];

			swordModel.translate_.y = 3.f;

			swordModel.scale_ = { 0.25f,1.f,0.25f };


			auto &sword = bone->boneTransform_[boneModel_->GetIndex("Sword")];

			if (not animate->timer_.IsFinish()) {
				if (animate->animIndex_ == 0u) {
					sword.rotate_ = Quaternion::AnyAxisRotation(Vector3::right, 90._deg * SoLib::easeInOutSine(animate->timer_.GetProgress()));
				}
				else {
					sword.rotate_ = Quaternion::AnyAxisRotation(Vector3::right, 90._deg * SoLib::easeInOutSine(1.f - animate->timer_.GetProgress()));
				}
			}
		}


	}
}

void ECS::System::BoneCollision::OnUpdate(::World *world, [[maybe_unused]] const float deltaTime) {

	for (const auto &[entity, bone, weapon] : world->view<ECS::BoneTransformComp, ECS::WeaponComp>()) {
		auto matrixArray = boneModel_->CalcTransMat(bone->boneTransform_);

		boneModel_->Draw(matrixArray);

		weapon->collision_.centor = *reinterpret_cast<Vector3 *>(matrixArray[boneModel_->GetIndex("Sword", 0)].m[3].data());
	}

}

void ECS::System::FollowCameraUpdate::OnUpdate(::World *world, [[maybe_unused]] const float deltaTime) {
	auto *inputManager = Input::GetInstance();

	for (const auto &[plEntity, player, pos] : world->view<ECS::PlayerTag, ECS::PositionComp>()) {


		for (const auto &[entity, followCamera, cameraPos] : world->view<ECS::FollowCamera, ECS::PositionComp>()) {
			*cameraPos = pos->position_;

			followCamera->rotation_.y += inputManager->GetXInput()->GetState()->stickR_.x * 90._deg * deltaTime;

			followCamera->TransferData(*CameraManager::GetInstance()->GetCamera("FollowCamera"), *cameraPos);
		}
	}
}

void ECS::System::SlideFollowCameraUpdate::OnUpdate(::World *world, [[maybe_unused]] const float deltaTime)
{

	for (const auto &[plEntity, player, pos] : world->view<ECS::PlayerTag, ECS::PositionComp>()) {


		for (const auto &[entity, followCamera, cameraPos] : world->view<ECS::FollowCamera, ECS::PositionComp>()) {
			*cameraPos = pos->position_;

			followCamera->TransferData(*CameraManager::GetInstance()->GetCamera("FollowCamera"), *cameraPos);
		}
	}
}

void ECS::System::MakeTransMatrix::OnUpdate(::World *world, [[maybe_unused]] const float deltaTime) {

	for (const auto &[entity, scale, quate, pos, mat] : world->view<ECS::ScaleComp, ECS::QuaternionRotComp, ECS::PositionComp, ECS::TransformMatComp>()) {

		mat->transformMat_ = SoLib::Math::Affine(*scale, quate->quateRot_.Normalize(), *pos);

	}

}

void ECS::System::DrawHelthBar::OnUpdate(::World *world, [[maybe_unused]] const float deltaTime) {
	for (const auto &[entity, plTag, health] : world->view<ECS::PlayerTag, ECS::HealthComp>()) {

		healthBar_->SetPercent(health->CalcPercent());

	}
}
