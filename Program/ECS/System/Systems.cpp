#include "Systems.h"
#include "../World/World.hpp"
#include "../Entity/EntityManager.hpp"

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
	for (const auto &[entity, player, plPos] : world->view<const ECS::PlayerTag, const ECS::PositionComp>()) {
		playerPos = *plPos;
	}

	for (const auto &[entity, enemy, pos, rotate] : world->view<ECS::EnemyTag, ECS::PositionComp, ECS::QuaternionRotComp>()) {
		Vector3 direction = (playerPos - pos->position_).Nomalize() * (100.f * deltaTime * deltaTime);
		direction.y = 0.f;
		pos->position_ += direction;

		rotate->quateRot_ = Quaternion::LookAt(direction);

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
