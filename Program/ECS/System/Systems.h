#pragma once
#include "ISystem.h"
#include "../Component/Component.hpp"
#include "../../Header/Object/Ground.h"
#include "../../Engine/DirectBase/Base/Audio.h"
#include "../../Engine/DirectBase/Model/Model.h"
#include "../../Header/Object/HealthBar.h"

namespace ECS {
	namespace System {

		class CheckAliveTime : public ISystem {

			bool IsConstSystem() const override { return false; }
			void OnUpdate(::World *world, const float deltaTime) override;

		};
		class CheckHealthDie : public ISystem {

			bool IsConstSystem() const override { return false; }
			void OnUpdate(::World *world, const float deltaTime) override;

		};

		class AddAliveTime : public ISystem {

			bool IsConstSystem() const override { return false; }
			void OnUpdate(::World *world, const float deltaTime) override;

		};

		class AddCoolTime : public ISystem {

			bool IsConstSystem() const override { return false; }
			void OnUpdate(::World *world, const float deltaTime) override;

		};

		class AnimateUpdate : public ISystem {

			bool IsConstSystem() const override { return false; }
			void OnUpdate(::World *world, const float deltaTime) override;

		};

		class ModelAnimatorUpdate : public ISystem {

			bool IsConstSystem() const override { return false; }
			void OnUpdate(::World *world, const float deltaTime) override;

		};

		class SkinModelUpdate : public ISystem {

			bool IsConstSystem() const override { return false; }
			void OnUpdate(::World *world, const float deltaTime) override;

		};

		class ColorLerp : public ISystem {

			bool IsConstSystem() const override { return false; }
			void OnUpdate(::World *world, const float deltaTime) override;

		};

		class AddGravity : public ISystem {

			bool IsConstSystem() const override { return false; }
			void OnUpdate(::World *world, const float deltaTime) override;

		};
		class AirResistanceSystem : public ISystem {

			bool IsConstSystem() const override { return false; }
			void OnUpdate(::World *world, const float deltaTime) override;

		};
		class MovePosition : public ISystem {
		public:

			bool IsConstSystem() const override { return false; }
			void OnUpdate(::World *world, const float deltaTime) override;
		};

		class EnemyMove : public ISystem {
		public:

			bool IsConstSystem() const override { return false; }
			void OnUpdate(::World *world, const float deltaTime) override;
		};

		class EnemyAttack : public ISystem {
		public:
			// 接触時の関数
			std::function<void(void)> hitFunc_;
			static EnemyAttack Create(std::function<void(void)> hitFunc) {
				EnemyAttack result;
				result.hitFunc_ = hitFunc;
				return result;
			}

			bool IsConstSystem() const override { return false; }
			void OnUpdate(::World *world, const float deltaTime) override;
		};

		class FallCollision : public ISystem {
		public:
			Ground *ground_;
			static FallCollision Create(Ground *ground) {
				FallCollision result;
				result.ground_ = ground;
				return result;
			}

			bool IsConstSystem() const override { return false; }
			void OnUpdate(::World *world, const float deltaTime) override;
		};

		class WeaponCollision : public ISystem {
		public:
			WeaponCollision() = default;
			WeaponCollision(const WeaponCollision &) = default;
			WeaponCollision(Audio::SoundHandle sound, Model *model) :sound_(sound), model_(model) {}

			static WeaponCollision Create(Audio::SoundHandle sound, Model *model) { return WeaponCollision{ sound, model }; }

			Audio::SoundHandle sound_;
			Model *model_;

			bool IsConstSystem() const override { return false; }
			void OnUpdate(::World *world, const float deltaTime) override;
		};

		class PlayerMove : public ISystem {
		public:

			bool IsConstSystem() const override { return false; }
			void OnUpdate(::World *world, const float deltaTime) override;
		};


		class PlayerAttack : public ISystem {
		public:
			Model *attackModel_;
			static PlayerAttack Create(Model *model) {
				PlayerAttack result;
				result.attackModel_ = model;

				return result;
			}

			bool IsConstSystem() const override { return false; }
			void OnUpdate(::World *world, const float deltaTime) override;
		};

		class BillboardCalc : public ISystem {
		public:

			bool IsConstSystem() const override { return false; }
			void OnUpdate(::World *world, const float deltaTime) override;
		};
		class BoneAnimationCalc : public ISystem {
		public:
			BoneModel *boneModel_;

			static BoneAnimationCalc Create(BoneModel *bone) {
				BoneAnimationCalc result;
				result.boneModel_ = bone;
				return result;
			}

			bool IsConstSystem() const override { return false; }
			void OnUpdate(::World *world, const float deltaTime) override;
		};

		class BoneCollision : public ISystem {
		public:
			BoneModel *boneModel_;

			static BoneCollision Create(BoneModel *bone) {
				BoneCollision result;
				result.boneModel_ = bone;
				return result;
			}

			bool IsConstSystem() const override { return false; }
			void OnUpdate(::World *world, const float deltaTime) override;
		};

		class ModelDrawer : public ISystem {
		public:

			bool IsConstSystem() const override { return false; }
			void OnUpdate(::World *world, const float deltaTime) override;
		};

		class BoneDrawer : public ISystem {
		public:
			BoneModel *boneModel_;

			static BoneDrawer Create(BoneModel *bone) {
				BoneDrawer result;
				result.boneModel_ = bone;
				return result;
			}

			bool IsConstSystem() const override { return false; }
			void OnUpdate(::World *world, const float deltaTime) override;
		};

		class FollowCameraUpdate : public ISystem {
		public:

			bool IsConstSystem() const override { return false; }
			void OnUpdate(::World *world, const float deltaTime) override;
		};

		class SlideFollowCameraUpdate : public ISystem {
		public:

			bool IsConstSystem() const override { return false; }
			void OnUpdate(::World *world, const float deltaTime) override;
		};

		class CalcTransMatrix : public ISystem {
		public:

			bool IsConstSystem() const override { return false; }
			void OnUpdate(::World *world, const float deltaTime) override;
		};

		class DrawHelthBar : public ISystem {
		public:
			HealthBar *healthBar_ = nullptr;
			static DrawHelthBar Create(HealthBar *healthBar) {
				DrawHelthBar result;
				result.healthBar_ = healthBar;
				return result;
			}

			bool IsConstSystem() const override { return false; }
			void OnUpdate(::World *world, const float deltaTime) override;
		};

		class DrawEnemyHelthBar : public ISystem {
		public:
			inline static constexpr uint32_t kDrawCount_ = 50u;

			std::array<std::unique_ptr<HealthBar>, kDrawCount_> *healthBar_ = nullptr;
			uint32_t drawCount_{};
			static DrawEnemyHelthBar Create(std::array<std::unique_ptr<HealthBar>, kDrawCount_> *healthBar) {
				DrawEnemyHelthBar result;
				result.healthBar_ = healthBar;
				return result;
			}

			bool IsConstSystem() const override { return false; }
			void OnUpdate(::World *world, const float deltaTime) override;
		};

		class CursorDrawer : public ISystem {
		public:

			bool IsConstSystem() const override { return false; }
			void OnUpdate(::World *world, const float deltaTime) override;
		};

		class CalcParentTransform : public ISystem {
		public:

			bool IsConstSystem() const override { return false; }
			void OnUpdate(::World *world, const float deltaTime) override;
		};


	}
}