/// @file NewSystems.cpp
/// @brief ECSのシステムの実装
/// @author ONUKI seiya
#include "NewSystems.h"
#include "../World/ComponentArray/Chunk.h"
#include "../World/NewWorld.h"
#include "../../Engine/DirectBase/Input/Input.h"
#include "../../DirectBase/Render/CameraManager.h"

namespace ECS::System::Par {

	void CheckAliveTime::Execute(const World *const, const float)
	{
		auto &[aliveTime, lifeLimit] = onlyRead_;
		auto &[isAlive] = onlyWrite_;

		if (lifeLimit.lifeLimit_ >= 0.f) {
			// 寿命を超過していたら
			if (lifeLimit.lifeLimit_ < aliveTime.aliveTime_) {
				// 死ぬ
				isAlive.isAlive_ = false;
			}
		}
	}

	void CheckHealthDie::Execute(const World *const, const float)
	{
		auto &[health] = onlyRead_;
		auto &[isAlive] = onlyWrite_;
		// もし体力が0以下なら
		if (health.IsDead()) {

			// 死ぬ
			isAlive.isAlive_ = false;
		}
	}

	void AddAliveTime::Execute(const World *const, const float deltaTime)
	{
		auto &[aliveTime] = readWrite_;
		// 生存時間を加算
		aliveTime.aliveTime_ += deltaTime;
	}

	void AddCoolTime::Execute(const World *const, const float deltaTime)
	{
		auto &[coolTime] = readWrite_;
		// 生存時間を加算
		coolTime.cooltime_.Update(deltaTime);
	}

	void CalcInvincibleTime::Execute(const World *const, const float deltaTime)
	{
		auto &[invincible] = readWrite_;
		// 生存時間を加算
		invincible.timer_.Update(deltaTime);
	}

	void AnimateUpdate::Execute(const World *const, const float deltaTime)
	{
		auto &[state] = readWrite_;
		// 現在の状態のタイマーを進める
		state.stateTimer_ += deltaTime;
	}

	void ModelAnimatorUpdate::Execute(const World *const, [[maybe_unused]] const float deltaTime)
	{
		const auto &[model, animator] = readWrite_;
		if (animator.animatior_.GetDeltaTimer().IsFinish()) {
			animator.animatior_.SetAnimation(animator.animateList_[0]);
			animator.animatior_.Start(true);
		}
		animator.animatior_.Update(deltaTime);
	}

	void SkinModelUpdate::Execute(const World *const, const float)
	{
		const auto &[model, animator] = readWrite_;
		model.skinModel_->Update(**animator.animatior_.GetAnimation(), animator.animatior_.GetDeltaTimer().GetNowFlame());
	}

	void ColorLerp::Execute(const World *const, const float)
	{
		auto &[aliveTime, lifelimit, colorLerp, color] = readWrite_;
		color.color_ = colorLerp.EaseColor(aliveTime.aliveTime_ / lifelimit.lifeLimit_);

	}

	void AddGravity::Execute(const World *const, const float deltaTime)
	{
		auto &[acceleration, gravity] = readWrite_;

		acceleration.acceleration_ += gravity.gravity_ * deltaTime;
	}

	void AirResistanceSystem::Execute(const World *const, const float)
	{
		auto &[acceleration, velocity, airResistance] = readWrite_;

		acceleration.acceleration_ -= velocity.velocity_ * airResistance.resistance;
	}

	void CalcAcceleration::Execute(const World *const, const float)
	{
		auto &[velocity, acceleration] = readWrite_;

		velocity.velocity_ += acceleration.acceleration_;
		acceleration.acceleration_ = {};
	}

	void MovePosition::Execute(const World *const, const float deltaTime)
	{
		auto &[pos, velocity] = readWrite_;

		pos.position_ += velocity.velocity_ * deltaTime;
	}

	void MoveCollisionPosition::Execute(const World *const, const float deltaTime)
	{
		auto &[pos, velocity] = readWrite_;


		pos.collision_.centor += velocity.velocity_ * deltaTime;
	}

	void CollisionPushOut::Execute(const World *const, const float)
	{


	}

	void CollisionPushOut::ExecuteOnce(const World *const world, const float)
	{
		collisions_.clear();
		auto chunks = world->GetAccessableChunk(Archetype::Generate<ECS::PositionComp, ECS::SphereCollisionComp, ECS::Rigidbody, ECS::EnemyTag>());

		if (chunks.empty()) {
			return;
		}

		for (const auto &cItr : chunks) {
			collisions_.push_back(cItr->View<ECS::PositionComp, ECS::SphereCollisionComp>());
		}

	}

	Vector3 EnemyMove::playerPos_{};
	void EnemyMove::Execute(const World *const, const float deltaTime)
	{

		auto &[enemy, pos, rotate, speed] = readWrite_;
		Vector3 direction = (playerPos_ - pos.position_);
		direction.y = 0.f;
		direction = direction.Normalize() * (speed.moveSpeed_ * deltaTime);
		pos.position_ += direction;

		rotate.quateRot_ = Quaternion::LookAt(direction);

	}

	void EnemyAttack::Execute(const World *const world, const float deltaTime)
	{
		// 時間が止まってたら終わる
		if (not deltaTime) {
			return;
		}

		auto &[playerTag, playerPos, playerHealth, playerCollision, playerAcceleration, invincibleTime] = readWrite_;


		// 当たり判定を取得
		auto plColl = playerCollision.collision_;
		plColl.centor += *playerPos;

		Archetype archetype;
		archetype.AddClassData<ECS::EnemyTag, ECS::PositionComp, ECS::SphereCollisionComp, ECS::AttackPower, ECS::AttackCooltime>();


		auto enemyView = world->GetAccessableChunk(archetype).View<ECS::PositionComp, ECS::SphereCollisionComp, ECS::AttackPower, ECS::AttackCooltime>();

		for (auto [pos, collider, power, coolTime] : enemyView) {

			Sphere enColl = collider.collision_;
			enColl.centor += pos.position_;

			// クールタイムが終わっており、接触している場合
			if (not coolTime.cooltime_.IsActive() and Collision::IsHit(plColl, enColl)) {
				Vector3 diff = playerPos.position_ - pos.position_;
				// 高さを0にする
				diff.y = 0;
				// プレイヤに加速度を加算
				playerAcceleration.acceleration_ += diff.Normalize() * 10.f;

				// もし無敵時間中じゃないなら
				if (not invincibleTime.timer_.IsActive()) {

					// 体力を減算
					playerHealth.nowHealth_ -= power.power_;

					invincibleTime.timer_.Start();

				}

			}

		}

	}

	void LevelUp::Execute(const World *const, const float)
	{
	}

	void LevelUp::ExecuteOnce(const World *const world, const float)
	{
		Archetype archetype;
		archetype.AddClassData<ECS::SphereCollisionComp, ECS::KnockBackDirection, ECS::AttackPower>();
		const auto &chunks = world->GetAccessableChunk(archetype);
		expList_ = std::make_unique<ExpList>();
		expList_->size_ = chunks.Count();
		if (expList_->size_) {
			expList_->pos_ = std::move(chunks.GetRange<ECS::PositionComp>());
			expList_->isAlive_ = std::move(chunks.GetRange<ECS::IsAlive>());
		}
	}

	const Ground *FallCollision::ground_ = nullptr;
	void FallCollision::Execute(const World *const, const float)
	{
		auto &[collision, pos, velocity] = readWrite_;

		// 地面より座標が下なら
		if ((pos.position_.y + collision.collision_.centor.y - collision.collision_.radius) < ground_->hight_) {
			// 地面の上に当たり判定を上にする
			pos.position_.y = ground_->hight_ - collision.collision_.centor.y + collision.collision_.radius;
			// もし落下していたら
			if (velocity.velocity_.y < 0.f) {
				// 移動速度を0にする
				velocity.velocity_.y = 0.f;
			}
		}

	}

	void WeaponCollision::Execute(const World *const, const float)
	{
		auto &[enemy, pos, collision, health, invincible, counter] = readWrite_;
		// 無敵時間なら終わり
		if (invincible.timer_.IsActive()) {
			return;
		}

		Sphere sphere = collision.collision_;
		sphere.centor += pos;
		for (uint32_t i = 0; i < attackCollisions_->size_; i++) {
			auto [sphereComp, knockBack, power] = attackCollisions_->collisionData_[i];

			// 攻撃判定が当たってたら検知
			if (Collision::IsHit(sphere, sphereComp.collision_)) {
				// 体力を減らす
				health.nowHealth_ -= power.power_;
				counter.damageCount_ += power.power_;
				counter.damageRemainTime_ = 1.f;

				// ノックバックが指定されてなかったら
				if (knockBack.diff_ == Vector2::zero) {
					// 相対座標で押し出す
					pos.position_ += (pos.position_ - sphereComp.collision_.centor) * knockBack.diffPower_.Random();
					pos.position_.y = 0.f;
				}
				invincible.timer_.Start(0.25f);
				break;

			}
		}

	}

	void WeaponCollision::ExecuteOnce(const World *const world, const float)
	{
		const auto &chunks = world->GetAccessableChunk(Archetype::Generate<ECS::SphereCollisionComp, ECS::KnockBackDirection, ECS::AttackPower>());
		if (not attackCollisions_) { attackCollisions_ = std::make_unique<AttackCollisions>(); }
		attackCollisions_->size_ = chunks.Count();
		if (attackCollisions_->size_) {
			{

				attackCollisions_->collisionData_.resize(attackCollisions_->size_);
				size_t index = 0;
				for (const auto &chunk : chunks) {
					auto view = chunk->View<ECS::SphereCollisionComp, ECS::KnockBackDirection, ECS::AttackPower>();

					std::copy(view.begin(), view.end(), attackCollisions_->collisionData_.begin() + index);
					index += chunk->size();
				}
			}

		}
	}

	void PlayerMove::Execute(const World *const, const float deltaTime)
	{
		auto *inputManager = SolEngine::Input::GetInstance();
		auto *const xInput = inputManager->GetXInput();
		auto *const dInput = inputManager->GetDirectInput();

		auto &[pos, quateRot, acceleration, input, animator, isLanding, attackSt, attackCooltime] = readWrite_;

		// 左スティックの入力
		Vector2 inputLs = xInput->GetState()->stickL_;
		if (inputLs == Vector2::zero) {
			if (dInput->IsPress(DIK_A)) { inputLs.x -= 1; }
			if (dInput->IsPress(DIK_D)) { inputLs.x += 1; }
			if (dInput->IsPress(DIK_W)) { inputLs.y += 1; }
			if (dInput->IsPress(DIK_S)) { inputLs.y -= 1; }
			inputLs = inputLs.Normalize();
		}
		// 3次元的に解釈した入力
		const Vector3 lInput3d{ inputLs.x,0.f,inputLs.y };
		// カメラの向きに回転したベクトル
		const Vector3 &rotateInput = lInput3d /** Quaternion::AnyAxisRotation(Vector3::up, camera->rotation_.y)*/;
		// 回転したベクトルを使って移動
		pos.position_ += rotateInput * (10.f * deltaTime);

		// ステージの半径
		static constexpr float kStageRadius = 43.f;

		pos.position_.x = std::clamp(pos.position_.x, -kStageRadius, kStageRadius);
		pos.position_.z = std::clamp(pos.position_.z, -kStageRadius, kStageRadius);

		pos.position_.y = 0.f;

		EnemyMove::playerPos_ = pos;

		// 右スティックの入力
		Vector2 inputRs = inputManager->GetXInput()->GetState()->stickR_;
		// キーボード対応
		if (inputRs == Vector2::zero) {
			if (dInput->IsPress(DIK_LEFT)) { inputRs.x -= 1; }
			if (dInput->IsPress(DIK_RIGHT)) { inputRs.x += 1; }
			if (dInput->IsPress(DIK_UP)) { inputRs.y += 1; }
			if (dInput->IsPress(DIK_DOWN)) { inputRs.y -= 1; }
			inputRs = inputRs.Normalize();
		}
		// もし角度が0でなければ
		if (inputRs != Vector2::zero) {
			// ハーフべクトルを定義
			Vector2 halfVector = (Vector2::up + inputRs.Normalize()).Normalize();
			// もし真後ろなら横を代入しておく
			if (halfVector == Vector2::zero) {
				halfVector = Vector2::right;
			}
			// ベクトルを代入する
			quateRot.quateRot_ = Quaternion{ Vector3::up * -(Vector2::up ^ halfVector), Vector2::up * halfVector };
		}
	}
	Model *PlayerAttack::attackModel_ = nullptr;
	void PlayerAttack::Execute(const World *const, const float)
	{
		// static ParticleManager *const particleManager = ParticleManager::GetInstance();

		auto &[pos, quateRot, attackSt, attCT, attColl, cursor, modelAnimator, state, exp] = readWrite_;
		// クールタイムが終わってたら
		if (attCT.cooltime_.IsFinish()) {
			// 再度開始
			attCT.cooltime_.Start();

			//// 攻撃の座標を設定
			//attColl.collision_.centor = quateRot.quateRot_.GetFront() * attackSt.offset_ + pos;
			//// 攻撃の半径を設定
			//attColl.collision_.radius = attackSt.radius_;

			//// 攻撃判定を有効化
			attColl.isActive_ = true;

			//auto particle = particleManager->AddParticle<SimpleParticle>(attackModel_, attColl.collision_.centor + Vector3{ .y = 0.1f });
			//particle->SetAliveTime(0.5f);
			//particle->transform_.rotate = SoLib::MakeQuaternion({ 90._deg,0,0 });
			//particle->transform_.scale = Vector3::one * (attColl.collision_.radius * 2.f);
			//particle->color_ = 0xFF5555FF;

			state.ChangeState(static_cast<uint32_t>(EntityState::PlayerState::kAttack));
			modelAnimator.animatior_.SetAnimation(modelAnimator.animateList_[static_cast<uint32_t>(EntityState::PlayerState::kAttack)]);
			modelAnimator.animatior_.Start();

		}
		// 終わってなかったら
		else {
			// 攻撃判定を無効化
			attColl.isActive_ = false;
		}

		//// 攻撃タイミングをカーソルに代入
		//cursor.progress_ = attCT.cooltime_.GetProgress();

	}

	void PlayerShooterUpdate::Execute(const World *const, const float deltaTime)
	{
		auto &[arrowShooter, stoneShooter] = readWrite_;
		arrowShooter.Update(deltaTime);
		stoneShooter.Update(deltaTime);

	}

	void PlayerAreaAttack::Execute(const World *const, const float)
	{
		auto &[pos, quateRot, attackCircle] = readWrite_;
		auto &[lifeLimit, aliveTime] = onlyRead_;



	}

	void ModelDrawer::Execute(const World *const, const float)
	{
		static ModelHandleListManager *const blockManager = ModelHandleListManager::GetInstance();

		auto &[transform, model] = readWrite_;

		blockManager->AddBox(model.model_, { .transMat_ = transform });
	}

	void SkinModelDrawer::Execute(const World *const, const float)
	{
		static SkinModelHandleListManager *const skinModelRender_ = SkinModelHandleListManager::GetInstance();

		auto &[transform, model, skinModel] = readWrite_;
		skinModelRender_->AddBox({ model.model_, skinModel.skinModel_ }, { .transMat_ = transform });
	}

	void SlideFollowCameraUpdate::Execute(const World *const world, const float)
	{
		std::list<
			const ECS::PositionComp *
		> posList{};

		Archetype archetype;
		archetype.AddClassData<ECS::PlayerTag, ECS::PositionComp>();
		for (auto *const chunk : world->GetAccessableChunk(archetype)) {
			auto posRange = chunk->GetComponent<ECS::PositionComp>();
			for (auto &pos : posRange) {
				posList.push_back(&pos);
			}
		}

		// もしなにもなかったらおわる
		if (posList.size() == 0) { return; }

		auto &[followCamera, cameraPos] = readWrite_;

		*cameraPos = posList.front()->position_;

		followCamera.TransferData(*SolEngine::CameraManager::GetInstance()->GetCamera("FollowCamera"), *cameraPos);
	}

	void CalcTransMatrix::Execute(const World *const, const float)
	{
		const auto &[scale, rot, pos] = onlyRead_;
		auto &[transMat] = onlyWrite_;

		transMat = SoLib::Affine(scale, rot.quateRot_.Normalize(), pos);

	}

	void CalcEulerTransMatrix::Execute(const World *const, const float)
	{
		const auto &[scale, rot, pos] = onlyRead_;
		auto &[transMat] = onlyWrite_;

		transMat = SoLib::Affine(scale, rot.rotate_, pos);
	}
	HealthBar *DrawHelthBar::healthBar_ = nullptr;
	void DrawHelthBar::Execute(const World *const, const float)
	{
		auto &[plTag, health] = readWrite_;
		healthBar_->SetPercent(health.CalcPercent());
	}

	std::array<std::unique_ptr<HealthBar>, DrawEnemyHelthBar::kDrawCount_> *DrawEnemyHelthBar::healthBar_ = nullptr;
	uint32_t DrawEnemyHelthBar::drawCount_ = 0;

	void DrawEnemyHelthBar::Init(std::array<std::unique_ptr<HealthBar>, kDrawCount_> *healthBar)
	{
		healthBar_ = healthBar;
	}

	void DrawEnemyHelthBar::Execute(const World *const, const float)
	{
		auto *const camera = SolEngine::CameraManager::GetInstance()->GetCamera("FollowCamera");

		const Matrix4x4 &vp = SolEngine::Render::MakeViewportMatrix({ 0,0 }, WinApp::kWindowWidth, WinApp::kWindowHeight);
		const Matrix4x4 &matVPVp = camera->matView_ * camera->matProjection_ * vp;
		auto &[enmTag, health, pos, barComp] = readWrite_;

		// 描画上限になった場合は終了
		if (drawCount_ >= healthBar_->size()) { return; }
		// もしMaxなら表示しない
		if (health.IsMax()) { return; }
		// 紐づいた経験値バー
		auto *const bar = healthBar_->at(drawCount_).get();

		// 画面上の場所
		const Vector3 screenPos = SolEngine::Render::WorldToScreen(*pos + barComp.offset_, matVPVp);
		// 描画場所を設定
		bar->SetCentor(screenPos.ToVec2());
		// 体力バーのサイズ
		bar->SetScale(*barComp.vDefaultBarScale_ * 0.25f);
		// 体力を設定
		bar->SetPercent(health.CalcPercent());

		// 描画個数を加算
		drawCount_++;

	}

	void CursorDrawer::Execute(const World *const, const float)
	{

		// カメラの逆行列
		const Matrix4x4 &billboardMat = SolEngine::CameraManager::GetInstance()->GetCamera()->matView_.GetRotate().InverseRT();

		static BlockManager *const blockManager = BlockManager::GetInstance();

		auto &[plTag, pos, rot, cursor] = readWrite_;

		// カーソルの座標
		Vector3 cursorPos = rot.quateRot_.GetFront() * cursor.offset_ + *pos + Vector3{ .y = 1.f };
		// 外枠の表示
		{
			// 描画する場所
			Matrix4x4 item = SoLib::Affine(Vector3::one * cursor.scale_, Vector3::zero, cursorPos);
			// ビルボード変換
			item *= billboardMat;
			// 座標を戻す
			item.GetTranslate() = cursorPos;

			// モデルの取得
			const Model *const model = cursor.model_;
			// 無かったら終わり
			if (not model) { return; }

			// 描画
			blockManager->AddBox(model, IBlock{ .transMat_ = item });
		}

		// 描画モデルがあり、倍率が有効な値である場合
		if (cursor.inModel_ && cursor.progress_) {
			// 描画する場所
			Matrix4x4 item = SoLib::Affine(Vector3::one * (cursor.scale_ * cursor.progress_), Vector3::zero, cursorPos);
			// ビルボード変換
			item *= billboardMat;
			// 座標を戻す
			item.GetTranslate() = cursorPos;

			// 内部のモデルの取得
			const Model *const model = cursor.inModel_;
			// 無かったら終わり
			if (not model) { return; }

			// 描画
			blockManager->AddBox(model, IBlock{ .transMat_ = item, .color_ = 0xFFFFFF55 });
		}


	}
	uint32_t ExpGaugeDrawer::prevLevel_ = 0u;
	LevelUP *ExpGaugeDrawer::levelUp_ = nullptr;
	HealthBar *ExpGaugeDrawer::expBar_ = nullptr;
	void ExpGaugeDrawer::Execute(const World *const, const float)
	{
		const auto &[exp] = readWrite_;

		if (exp.exp_ >= exp.needExp_(exp.level_)) {

			prevLevel_ = exp.level_;

			while (exp.exp_ >= exp.needExp_(exp.level_)) {
				exp.exp_ -= exp.needExp_(exp.level_);
				exp.level_++;
			}
			levelUp_->Open();
		}

		expBar_->SetPercent(static_cast<float>(exp.exp_) / exp.needExp_(exp.level_));

	}

	void CalcParentTransform::Execute(const World *const, const float)
	{
		//auto &[transMat, parent] = readWrite_;
		//// 親の有効期限が切れてたら飛ばす
		//if (not parent.parent_) { return; }
		//// 親のエンティティから､行列コンポーネントを取得する
		//auto parentTransMat = parent.parent_.chunk_->GetComp<ECS::TransformMatComp>(parent.parent_.totalIndex_);
		//// 親の行列を乗算する
		//transMat.transformMat_ *= parentTransMat->transformMat_;
	}
	void DamageUpdate::Execute(const World *const, const float deltaTime)
	{
		auto &[damage] = this->readWrite_;
		if (damage.damageRemainTime_ >= 0.f) {
			damage.damageRemainTime_ -= deltaTime;
		}
		else {
			damage.damageCount_ = 0;
		}

	}
	void CollectionExpOrb::Execute(const World *const, const float)
	{
		auto [exp, position] = readWrite_;




	}
	void CollectionExpOrb::ExecuteOnce(const World *const world, const float)
	{
		const auto &chunks = world->GetAccessableChunk(Archetype::Generate<ECS::PlayerTag>());
		if (not playerPos_) { playerPos_ = std::make_unique<PlayerPos>(); }
		if (chunks.empty()) { playerPos_->pos_ = std::nullopt; return; }

		auto compRange = chunks.front()->GetComponent<ECS::PositionComp>();
		if (not compRange.IsActive()) { playerPos_->pos_ = std::nullopt; return; }

		playerPos_->pos_ = compRange.front();

	}
	void StoneWeaponCollision::Execute(const World *const, const float)
	{
		if (not shareData_ or not shareData_->playerPos_) { return; }
		auto [aliveTime, coll, stone] = readWrite_;

		Vector3 front = Quaternion::AnyAxisRotation(Vector3::up, SoLib::Angle::Mod(shareData_->playerAliveTime_ * stone.rotateSpeed_ + stone.angleOffset_)).GetFront() * stone.distance_;
		coll.collision_.centor = front + *shareData_->playerPos_;

	}
	void StoneWeaponCollision::ExecuteOnce(const World *const world, const float)
	{
		const auto &chunks = world->GetAccessableChunk(Archetype::Generate<ECS::PlayerTag, ECS::PositionComp, ECS::AliveTime>());
		if (not shareData_ or not shareData_->playerPos_) { shareData_ = std::make_unique<ShareData>(); }
		if (chunks.empty() or not chunks.front() or not chunks.front()->size()) { shareData_->playerPos_ = std::nullopt; return; }

		auto compRange = chunks.front()->View<ECS::PositionComp, ECS::AliveTime>();
		if (compRange) { shareData_->playerPos_ = std::nullopt; return; }

		auto [pos, aliveTime] = compRange.front();

		shareData_->playerPos_ = pos;
		shareData_->playerAliveTime_ = aliveTime.aliveTime_;

	}

	void FallingStoneUpdate::Execute(const World *const, const float)
	{
		const auto &[fall, isAlive, coll] = readWrite_;

		if (coll.collision_.centor.y < -coll.collision_.radius) {
			isAlive.isAlive_ = false;
		}

	}
}