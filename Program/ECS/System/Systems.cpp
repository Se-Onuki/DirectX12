#include "Systems.h"
#include "../World/World.hpp"
#include "../Entity/EntityManager.hpp"
#include "../../Header/Object/Particle/TestParticle.h"
#include "../../Engine/DirectBase/Input/Input.h"
#include "../../Engine/DirectBase/Render/CameraAnimations/CameraManager.h"
#include "../../Header/Object/Particle/SimpleParticle.h"

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


void ECS::System::AddCoolTime::OnUpdate(::World *world, const float deltaTime)
{
	for (const auto &[entity, coolTime] : world->view<ECS::AttackCooltime>()) {
		// 生存時間を加算
		coolTime->cooltime_.Update(deltaTime);
	}

}


void ECS::System::AnimateUpdate::OnUpdate(::World *world, [[maybe_unused]] const float deltaTime) {

	for (auto [entity, state] : world->view<ECS::EntityState>()) {
		// 現在の状態のタイマーを進める
		state->stateTimer_ += deltaTime;
	}

}

void ECS::System::ModelAnimatorUpdate::OnUpdate(::World *world, const float deltaTime)
{
	for (const auto &[entity, model, animator] : world->view<ECS::ModelComp, ECS::ModelAnimator>()) {
		if (animator->animatior_.GetDeltaTimer().IsFinish()) {
			animator->animatior_.SetAnimation(animator->animateList_[0]);
			animator->animatior_.Start(true);
		}
		animator->animatior_.Update(deltaTime, model->model_);
	}

}

void ECS::System::SkinModelUpdate::OnUpdate(::World *world, [[maybe_unused]] const float deltaTime)
{
	for (const auto &[entity, model, animator] : world->view<ECS::SkinModel, ECS::ModelAnimator>()) {
		model->skinModel_->Update(*animator->animatior_.GetAnimation(), animator->animatior_.GetDeltaTimer().GetNowFlame());
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
	for (const auto &[entity, playerTag, playerPos, playerHealth, playerCollision, playerAcceleration] : world->view<const ECS::PlayerTag, const ECS::PositionComp, ECS::HealthComp, const ECS::SphereCollisionComp, ECS::AccelerationComp>()) {
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

	for (const auto &[entity, enemy, pos, coll, attackPow, attackCT] : world->view<const ECS::EnemyTag, const ECS::PositionComp, const ECS::SphereCollisionComp, ECS::AttackPower, ECS::AttackCooltime>()) {
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

			if (hitFunc_) {
				hitFunc_();
			}

		}
	}
}


void ECS::System::FallCollision::OnUpdate(::World *world, [[maybe_unused]] const float deltaTime) {

	for (const auto &[entity, collision, pos, velocity] : world->view<ECS::SphereCollisionComp, ECS::PositionComp, ECS::VelocityComp>()) {
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
		std::list<
			std::tuple<
			const ECS::AttackCollisionComp *,
			const ECS::AttackPower *
			>
		> weaponList{};

		// 攻撃判定を保存
		for (const auto &[entity, weapon, damage] : world->view<const ECS::AttackCollisionComp, const ECS::AttackPower>()) {
			// 有効なら保存する
			if (weapon->isActive_) {
				weaponList.push_back({ weapon, damage });
			}
		}

		for (const auto &[entity, enemy, pos, collision, health] : world->view<ECS::EnemyTag, ECS::PositionComp, ECS::SphereCollisionComp, ECS::HealthComp>()) {

			Sphere sphere = collision->collision_;
			sphere.centor += *pos;

			for (const auto &[weapon, damage] : weaponList) {
				// 攻撃判定が当たってたら検知
				if (Collision::IsHit(sphere, weapon->collision_)) {
					// 体力を減らす
					health->nowHealth_ -= damage->power_;

					// ダメージ音を出す
					sound_.Play(false, 1.f);

					// パーティクルを出す
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

	for (const auto &[entity, pos, quateRot, acceleration, input, animator, isLanding, attackSt, attackCooltime] : world->view<ECS::PositionComp, ECS::QuaternionRotComp, ECS::AccelerationComp, ECS::InputFlagComp, ECS::ModelAnimator, ECS::IsLanding, ECS::AttackStatus, ECS::AttackCooltime>()) {
		// const auto *const camera = CameraManager::GetInstance()->GetCamera("FollowCamera");
		// 
		// 左スティックの入力
		const Vector2 inputLs = inputManager->GetXInput()->GetState()->stickL_;
		// 3次元的に解釈した入力
		const Vector3 lInput3d{ inputLs.x,0.f,inputLs.y };
		// カメラの向きに回転したベクトル
		const Vector3 &rotateInput = lInput3d /** Quaternion::AnyAxisRotation(Vector3::up, camera->rotation_.y)*/;
		// 回転したベクトルを使って移動
		pos->position_ += rotateInput * (500.f * deltaTime * deltaTime);

		// 右スティックの入力
		const Vector2 inputRs = inputManager->GetXInput()->GetState()->stickR_;
		// 3次元的に解釈した入力
		const Vector3 rInput3d{ inputRs.x, 0.f,inputRs.y };

		if (rInput3d.LengthSQ()) {
			quateRot->quateRot_ = Quaternion::LookAt(rInput3d);
		}

		//if (animator->animatior_.GetDeltaTimer().IsFinish()) {
		if (attackCooltime->cooltime_.IsFinish()) {
			// animator->animatior_.Start(false);
		}
		//}
	}
}


void ECS::System::PlayerAttack::OnUpdate(::World *world, [[maybe_unused]] const float deltaTime)
{
	static ParticleManager *const particleManager = ParticleManager::GetInstance();

	for (const auto &[entity, pos, quateRot, attackSt, attCT, attColl, cursor, modelAnimator, state] : world->view<const ECS::PositionComp, const ECS::QuaternionRotComp, ECS::AttackStatus, ECS::AttackCooltime, ECS::AttackCollisionComp, ECS::CursorComp, ECS::ModelAnimator, ECS::EntityState>()) {
		// クールタイムが終わってたら
		if (attCT->cooltime_.IsFinish()) {
			// 再度開始
			attCT->cooltime_.Start();

			// 攻撃の座標を設定
			attColl->collision_.centor = quateRot->quateRot_.GetFront() * attackSt->offset_ + **pos;
			// 攻撃の半径を設定
			attColl->collision_.radius = attackSt->radius_;

			// 攻撃判定を有効化
			attColl->isActive_ = true;

			auto particle = particleManager->AddParticle<SimpleParticle>(attackModel_, attColl->collision_.centor + Vector3{ .y = 0.1f });
			particle->SetAliveTime(0.5f);
			particle->transform_.rotate = SoLib::MakeQuaternion({ 90._deg,0,0 });
			particle->transform_.scale = Vector3::one * (attackSt->radius_ * 2.f);
			particle->color_ = 0xFF5555FF;

			state->ChangeState(static_cast<uint32_t>(EntityState::PlayerState::kAttack));
			modelAnimator->animatior_.SetAnimation(modelAnimator->animateList_[static_cast<uint32_t>(EntityState::PlayerState::kAttack)]);
			modelAnimator->animatior_.Start();

		}
		// 終わってなかったら
		else {
			// 攻撃判定を無効化
			attColl->isActive_ = false;
		}

		// 攻撃タイミングをカーソルに代入
		cursor->progress_ = attCT->cooltime_.GetProgress();
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
	for (const auto &[entity, scale, quate, pos, bone, animate] : world->view<ECS::ScaleComp, ECS::QuaternionRotComp, ECS::PositionComp, ECS::BoneTransformComp, ECS::EntityState>()) {

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


			//auto &sword = bone->boneTransform_[boneModel_->GetIndex("Sword")];

			/*if (not animate->timer_.IsFinish()) {
				if (animate->animIndex_ == 0u) {
					sword.rotate_ = Quaternion::AnyAxisRotation(Vector3::right, 90._deg * SoLib::easeInOutSine(animate->timer_.GetProgress()));
				}
				else {
					sword.rotate_ = Quaternion::AnyAxisRotation(Vector3::right, 90._deg * SoLib::easeInOutSine(1.f - animate->timer_.GetProgress()));
				}
			}*/
		}


	}
}

void ECS::System::BoneCollision::OnUpdate(::World *world, [[maybe_unused]] const float deltaTime) {

	for (const auto &[entity, bone, weapon] : world->view<ECS::BoneTransformComp, ECS::AttackCollisionComp>()) {
		auto matrixArray = boneModel_->CalcTransMat(bone->boneTransform_);

		weapon->collision_.centor = *reinterpret_cast<Vector3 *>(matrixArray[boneModel_->GetIndex("Sword", 0)].m[3].data());
	}

}

void ECS::System::ModelDrawer::OnUpdate(::World *world, [[maybe_unused]] const float deltaTime) {
	static BlockManager *const blockManager = BlockManager::GetInstance();
	static SkinModelListManager *const skinModelRender_ = SkinModelListManager::GetInstance();

	for (const auto &[entity, transform, model] : world->view<ECS::TransformMatComp, ECS::ModelComp>()) {
		auto [skinModel] = world->GetEntityManager()->GetComponent<ECS::SkinModel>(*entity);
		if (skinModel == nullptr) {	// スキンモデルのデータを持っていない場合は通常描画
			blockManager->AddBox(model->model_, { .transMat_ = *transform });
		}
		else {	// スキンモデルのデータを持っていた場合はスキンモデルの描画
			skinModelRender_->AddBox({ model->model_, skinModel->skinModel_ }, { .transMat_ = *transform });
		}
	}

}

void ECS::System::BoneDrawer::OnUpdate(::World *world, [[maybe_unused]] const float deltaTime) {

	for (const auto &[entity, bone] : world->view<ECS::BoneTransformComp>()) {
		auto matrixArray = boneModel_->CalcTransMat(bone->boneTransform_);

		boneModel_->Draw(matrixArray);
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

void ECS::System::CalcTransMatrix::OnUpdate(::World *world, [[maybe_unused]] const float deltaTime) {

	for (const auto &[entity, scale, pos, mat] : world->view<const ECS::ScaleComp, const ECS::PositionComp, ECS::TransformMatComp>()) {
		const auto [quateRot, eulerRot] = world->GetEntityManager()->GetComponent<const ECS::QuaternionRotComp, const ECS::RotateComp>(*entity);
		if (quateRot) {

			mat->transformMat_ = SoLib::Math::Affine(*scale, quateRot->quateRot_.Normalize(), *pos);
		}
		else if (eulerRot) {
			mat->transformMat_ = SoLib::Math::Affine(*scale, eulerRot->rotate_, *pos);
		}
	}

}

void ECS::System::DrawHelthBar::OnUpdate(::World *world, [[maybe_unused]] const float deltaTime) {
	for (const auto &[entity, plTag, health] : world->view<ECS::PlayerTag, ECS::HealthComp>()) {

		healthBar_->SetPercent(health->CalcPercent());

	}
}

void ECS::System::DrawEnemyHelthBar::OnUpdate(::World *world, [[maybe_unused]] const float deltaTime) {

	auto *const camera = CameraManager::GetInstance()->GetCamera("FollowCamera");

	const Matrix4x4 &vp = Render::MakeViewportMatrix({ 0,0 }, WinApp::kWindowWidth, WinApp::kWindowHeight);
	const Matrix4x4 &matVPVp = camera->matView_ * camera->matProjection_ * vp;
	drawCount_ = 0u;
	for (const auto &[entity, enmTag, health, pos, barComp] : world->view<ECS::EnemyTag, ECS::HealthComp, ECS::PositionComp, ECS::HealthBarComp>()) {

		// 描画上限になった場合は終了
		if (drawCount_ >= healthBar_->size()) { break; }
		// もしMaxなら表示しない
		if (health->IsMax()) { continue; }
		// 紐づいた経験値バー
		auto *const bar = healthBar_->at(drawCount_).get();

		// 画面上の場所
		const Vector3 screenPos = Render::WorldToScreen(**pos + barComp->offset_, matVPVp);
		// 描画場所を設定
		bar->SetCentor(screenPos.ToVec2());
		// 体力バーのサイズ
		bar->SetScale(*barComp->vDefaultBarScale_ * 0.25f);
		// 体力を設定
		bar->SetPercent(health->CalcPercent());

		// 描画個数を加算
		drawCount_++;
	}
}

void ECS::System::CursorDrawer::OnUpdate(::World *world, [[maybe_unused]] const float deltaTime)
{
	// カメラの逆行列
	const Matrix4x4 &billboardMat = CameraManager::GetInstance()->GetUseCamera()->matView_.GetRotate().InverseRT();

	static BlockManager *const blockManager = BlockManager::GetInstance();

	for (const auto &[entity, plTag, pos, rot, cursor] : world->view<ECS::PlayerTag, ECS::PositionComp, ECS::QuaternionRotComp, ECS::CursorComp>()) {

		// カーソルの座標
		Vector3 cursorPos = rot->quateRot_.GetFront() * cursor->offset_ + **pos + Vector3{ .y = 1.f };
		// 外枠の表示
		{
			// 描画する場所
			Matrix4x4 item = SoLib::Affine(Vector3::one * cursor->scale_, Vector3::zero, cursorPos);
			// ビルボード変換
			item *= billboardMat;
			// 座標を戻す
			item.GetTranslate() = cursorPos;

			// モデルの取得
			const Model *const model = cursor->model_;
			// 無かったら終わり
			if (not model) { continue; }

			// 描画
			blockManager->AddBox(model, IBlock{ .transMat_ = item });
		}

		// 描画モデルがあり、倍率が有効な値である場合
		if (cursor->inModel_ && cursor->progress_) {
			// 描画する場所
			Matrix4x4 item = SoLib::Affine(Vector3::one * (cursor->scale_ * cursor->progress_), Vector3::zero, cursorPos);
			// ビルボード変換
			item *= billboardMat;
			// 座標を戻す
			item.GetTranslate() = cursorPos;

			// 内部のモデルの取得
			const Model *const model = cursor->inModel_;
			// 無かったら終わり
			if (not model) { continue; }

			// 描画
			blockManager->AddBox(model, IBlock{ .transMat_ = item, .color_ = 0xFFFFFF55 });
		}
	}


}

void ECS::System::CalcParentTransform::OnUpdate(::World *world, [[maybe_unused]] const float deltaTime)
{
	auto *const entityManager = world->GetEntityManager();
	for (const auto &[entity, transMat, parent] : world->view<ECS::TransformMatComp, ECS::Parent>()) {
		auto [parentTransMat] = entityManager->GetComponent<ECS::TransformMatComp>(parent->parent_);
		if (parentTransMat) {

			transMat->transformMat_ *= parentTransMat->transformMat_;
		}
	}
}
