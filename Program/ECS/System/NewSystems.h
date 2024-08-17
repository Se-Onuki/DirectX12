#pragma once
#include "FunctionalSystem.h"
#include "../../Header/Object/Ground.h"
#include "../../Header/Object/HealthBar.h"
#include "../../Engine/DirectBase/Base/Audio.h"
#include "../../Engine/DirectBase/Model/Model.h"

namespace ECS {
	namespace System {
		namespace Par {
			class CheckAliveTime : public IFunctionalSystem {
			public:
				using ReadWrite = GetComponentHelper<ECS::AliveTime, ECS::LifeLimit, ECS::IsAlive>;
				CheckAliveTime(std::byte **ptr) : readWrite_(ReadWrite::Copy(ptr)) {}
				ReadWrite::Components readWrite_;

				void Execute() override;

			};
			class CheckHealthDie : public IFunctionalSystem {
			public:
				using ReadWrite = GetComponentHelper<ECS::HealthComp, ECS::IsAlive>;
				CheckHealthDie(std::byte **ptr) : readWrite_(ReadWrite::Copy(ptr)) {}
				ReadWrite::Components readWrite_;

				void Execute() override;

			};

			class AddAliveTime : public IFunctionalSystem {
			public:
				using ReadWrite = GetComponentHelper<ECS::AliveTime>;
				AddAliveTime(std::byte **ptr) : readWrite_(ReadWrite::Copy(ptr)) {}
				ReadWrite::Components readWrite_;

				void Execute() override;

			};

			class AddCoolTime : public IFunctionalSystem {
			public:
				using ReadWrite = GetComponentHelper<ECS::AttackCooltime>;
				AddCoolTime(std::byte **ptr) : readWrite_(ReadWrite::Copy(ptr)) {}
				ReadWrite::Components readWrite_;

				void Execute() override;

			};

			class AnimateUpdate : public IFunctionalSystem {
			public:
				using ReadWrite = GetComponentHelper<ECS::EntityState>;
				AnimateUpdate(std::byte **ptr) : readWrite_(ReadWrite::Copy(ptr)) {}
				ReadWrite::Components readWrite_;

				void Execute() override;

			};

			class ModelAnimatorUpdate : public IFunctionalSystem {
			public:
				using ReadWrite = GetComponentHelper<ECS::ModelComp, ECS::ModelAnimator>;
				ModelAnimatorUpdate(std::byte **ptr) : readWrite_(ReadWrite::Copy(ptr)) {}
				ReadWrite::Components readWrite_;

				void Execute() override;

			};

			class SkinModelUpdate : public IFunctionalSystem {
			public:
				using ReadWrite = GetComponentHelper<ECS::SkinModel, ECS::ModelAnimator>;
				SkinModelUpdate(std::byte **ptr) : readWrite_(ReadWrite::Copy(ptr)) {}
				ReadWrite::Components readWrite_;

				void Execute() override;

			};

			class ColorLerp : public IFunctionalSystem {
			public:
				using ReadWrite = GetComponentHelper<ECS::AliveTime, ECS::LifeLimit, ECS::ColorLarp, ECS::Color>;
				ColorLerp(std::byte **ptr) : readWrite_(ReadWrite::Copy(ptr)) {}
				ReadWrite::Components readWrite_;

				void Execute() override;

			};

			class AddGravity : public IFunctionalSystem {
			public:
				using ReadWrite = GetComponentHelper<ECS::AccelerationComp, ECS::GravityComp>;
				AddGravity(std::byte **ptr) : readWrite_(ReadWrite::Copy(ptr)) {}
				ReadWrite::Components readWrite_;

				void Execute() override;

			};
			class AirResistanceSystem : public IFunctionalSystem {
			public:
				using ReadWrite = GetComponentHelper<ECS::AccelerationComp, const ECS::VelocityComp, const ECS::AirResistance>;
				AirResistanceSystem(std::byte **ptr) : readWrite_(ReadWrite::Copy(ptr)) {}
				ReadWrite::Components readWrite_;

				void Execute() override;

			};
			class MovePosition : public IFunctionalSystem {
			public:
				using ReadWrite = GetComponentHelper<ECS::PositionComp, ECS::VelocityComp, ECS::AccelerationComp>;
				MovePosition(std::byte **ptr) : readWrite_(ReadWrite::Copy(ptr)) {}
				ReadWrite::Components readWrite_;

				void Execute() override;
			};

			class EnemyMove : public IFunctionalSystem {
			public:
				using ReadWrite = GetComponentHelper<ECS::EnemyTag, ECS::PositionComp, ECS::QuaternionRotComp>;
				EnemyMove(std::byte **ptr) : readWrite_(ReadWrite::Copy(ptr)) {}
				ReadWrite::Components readWrite_;

				static Vector3 playerPos_;

				void Execute() override;
			};

			class EnemyAttack : public IFunctionalSystem {
			public:
				using ReadWrite = GetComponentHelper<ECS::PlayerTag, ECS::PositionComp, ECS::HealthComp, ECS::SphereCollisionComp, ECS::AccelerationComp>;
				EnemyAttack(std::byte **ptr) : readWrite_(ReadWrite::Copy(ptr)) {}
				ReadWrite::Components readWrite_;

				// 接触時の関数
				inline static std::function<void(void)> hitFunc_;

				void Execute() override;
			};

			class FallCollision : public IFunctionalSystem {
			public:
				using ReadWrite = GetComponentHelper<ECS::SphereCollisionComp, ECS::PositionComp, ECS::VelocityComp>;
				FallCollision(std::byte **ptr) : readWrite_(ReadWrite::Copy(ptr)) {}
				ReadWrite::Components readWrite_;

				static const Ground *ground_;

				void Execute() override;
			};

			class WeaponCollision : public IFunctionalSystem {
			public:
				using ReadWrite = GetComponentHelper<ECS::EnemyTag, ECS::PositionComp, ECS::SphereCollisionComp, ECS::HealthComp>;
				WeaponCollision(std::byte **ptr) : readWrite_(ReadWrite::Copy(ptr)) {}
				ReadWrite::Components readWrite_;

				/*static Audio::SoundHandle sound_;
				static ::Model *model_;*/

				void Execute() override;
			};

			class PlayerMove : public IFunctionalSystem {
			public:
				using ReadWrite = GetComponentHelper<ECS::PositionComp, ECS::QuaternionRotComp, ECS::AccelerationComp, ECS::InputFlagComp, ECS::ModelAnimator, ECS::IsLanding, ECS::AttackStatus, ECS::AttackCooltime>;
				PlayerMove(std::byte **ptr) : readWrite_(ReadWrite::Copy(ptr)) {}
				ReadWrite::Components readWrite_;

				void Execute() override;
			};


			class PlayerAttack : public IFunctionalSystem {
			public:
				using ReadWrite = GetComponentHelper<ECS::PositionComp, ECS::QuaternionRotComp, ECS::AttackStatus, ECS::AttackCooltime, ECS::AttackCollisionComp, ECS::CursorComp, ECS::ModelAnimator, ECS::EntityState, ECS::Experience>;
				PlayerAttack(std::byte **ptr) : readWrite_(ReadWrite::Copy(ptr)) {}
				ReadWrite::Components readWrite_;

				static Model *attackModel_;

				void Execute() override;
			};

			class ModelDrawer : public IFunctionalSystem {
			public:
				static constexpr bool kIsSingleThread_ = true;
				using ReadWrite = GetComponentHelper<ECS::TransformMatComp, ECS::ModelComp>;
				ModelDrawer(std::byte **ptr) : readWrite_(ReadWrite::Copy(ptr)) {}
				ReadWrite::Components readWrite_;

				void Execute() override;
			};

			class SkinModelDrawer : public IFunctionalSystem {
			public:
				static constexpr bool kIsSingleThread_ = true;
				using ReadWrite = GetComponentHelper<ECS::TransformMatComp, ECS::ModelComp, ECS::SkinModel>;
				SkinModelDrawer(std::byte **ptr) : readWrite_(ReadWrite::Copy(ptr)) {}
				ReadWrite::Components readWrite_;

				void Execute() override;
			};

			class SlideFollowCameraUpdate : public IFunctionalSystem {
			public:
				using ReadWrite = GetComponentHelper<ECS::FollowCamera, ECS::PositionComp>;
				SlideFollowCameraUpdate(std::byte **ptr) : readWrite_(ReadWrite::Copy(ptr)) {}
				ReadWrite::Components readWrite_;

				void Execute() override;
			};

			class CalcTransMatrix : public IFunctionalSystem {
			public:
				using ReadWrite = GetComponentHelper<ECS::ScaleComp, ECS::QuaternionRotComp, ECS::PositionComp, ECS::TransformMatComp>;
				CalcTransMatrix(std::byte **ptr) : readWrite_(ReadWrite::Copy(ptr)) {}
				ReadWrite::Components readWrite_;

				void Execute() override;
			};

			class CalcEulerTransMatrix : public IFunctionalSystem {
			public:
				using ReadWrite = GetComponentHelper<ECS::ScaleComp, ECS::RotateComp, ECS::PositionComp, ECS::TransformMatComp>;
				CalcEulerTransMatrix(std::byte **ptr) : readWrite_(ReadWrite::Copy(ptr)) {}
				ReadWrite::Components readWrite_;

				void Execute() override;
			};

			class DrawHelthBar : public IFunctionalSystem {
			public:
				using ReadWrite = GetComponentHelper<ECS::PlayerTag, ECS::HealthComp>;
				DrawHelthBar(std::byte **ptr) : readWrite_(ReadWrite::Copy(ptr)) {}
				ReadWrite::Components readWrite_;

				static HealthBar *healthBar_;

				void Execute() override;
			};

			class DrawEnemyHelthBar : public IFunctionalSystem {
			public:
				static constexpr bool kIsSingleThread_ = true;
				using ReadWrite = GetComponentHelper<ECS::EnemyTag, ECS::HealthComp, ECS::PositionComp, ECS::HealthBarComp>;
				DrawEnemyHelthBar(std::byte **ptr) : readWrite_(ReadWrite::Copy(ptr)) {}
				ReadWrite::Components readWrite_;

				inline static constexpr uint32_t kDrawCount_ = 50u;

				static std::array<std::unique_ptr<HealthBar>, kDrawCount_> *healthBar_;
				static uint32_t drawCount_;

				static void Init(std::array<std::unique_ptr<HealthBar>, kDrawCount_> *healthBar);

				void Execute() override;
			};

			class CursorDrawer : public IFunctionalSystem {
			public:
				using ReadWrite = GetComponentHelper<ECS::PlayerTag, ECS::PositionComp, ECS::QuaternionRotComp, ECS::CursorComp>;
				CursorDrawer(std::byte **ptr) : readWrite_(ReadWrite::Copy(ptr)) {}
				ReadWrite::Components readWrite_;

				void Execute() override;
			};

			class ExpGaugeDrawer : public IFunctionalSystem {
			public:
				using ReadWrite = GetComponentHelper<ECS::AttackCooltime>;
				ExpGaugeDrawer(std::byte **ptr) : readWrite_(ReadWrite::Copy(ptr)) {}
				ReadWrite::Components readWrite_;

				static uint32_t prevLevel_;
				static SoLib::DeltaTimer levelUpTimer_;
				static Sprite *levelUI_;
				static HealthBar *expBar_;

				void Execute() override;
			};

			class CalcParentTransform : public IFunctionalSystem {
			public:
				using ReadWrite = GetComponentHelper<ECS::TransformMatComp, ECS::Parent>;
				CalcParentTransform(std::byte **ptr) : readWrite_(ReadWrite::Copy(ptr)) {}
				ReadWrite::Components readWrite_;

				void Execute() override;
			};

		}
	}
}
