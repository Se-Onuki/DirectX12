#pragma once
#include "FunctionalSystem.h"
#include "../../Header/Object/Ground.h"
#include "../../Header/Object/HealthBar.h"
#include "../../Engine/DirectBase/Base/Audio.h"
#include "../../Engine/DirectBase/Model/Model.h"
#include "../../../Header/Object/PlayerLevel/LevelUP.h"

namespace ECS {
	namespace System {
		namespace Par {
			class CheckAliveTime :public IJobEntity {
			public:
				OnlyRead<ECS::AliveTime, ECS::LifeLimit> onlyRead_;
				OnlyWrite<ECS::IsAlive> onlyWrite_;
				using DataBase = DataBase<decltype(onlyRead_), decltype(onlyWrite_)>;

				void Execute(const World *const, const float);

			};
			class CheckHealthDie :public IJobEntity {
			public:
				OnlyRead<ECS::HealthComp> onlyRead_;
				OnlyWrite<ECS::IsAlive> onlyWrite_;
				using DataBase = DataBase<decltype(onlyRead_), decltype(onlyWrite_)>;

				void Execute(const World *const, const float);

			};

			class AddAliveTime :public IJobEntity {
			public:

				ReadAndWrite<ECS::AliveTime> readWrite_;
				using DataBase = DataBase<decltype(readWrite_)>;

				void Execute(const World *const, const float);

			};

			class AddCoolTime :public IJobEntity {
			public:
				ReadAndWrite<ECS::AttackCooltime> readWrite_;
				using DataBase = DataBase<decltype(readWrite_)>;

				void Execute(const World *const, const float);

			};

			class AnimateUpdate :public IJobEntity {
			public:
				ReadAndWrite<ECS::EntityState> readWrite_;
				using DataBase = DataBase<decltype(readWrite_)>;

				void Execute(const World *const, const float);

			};

			class ModelAnimatorUpdate :public IJobEntity {
			public:
				ReadAndWrite<ECS::ModelComp, ECS::ModelAnimator> readWrite_;
				using DataBase = DataBase<decltype(readWrite_)>;

				void Execute(const World *const, const float);

			};

			class SkinModelUpdate :public IJobEntity {
			public:
				ReadAndWrite<ECS::SkinModel, ECS::ModelAnimator> readWrite_;
				using DataBase = DataBase<decltype(readWrite_)>;

				void Execute(const World *const, const float);

			};

			class ColorLerp :public IJobEntity {
			public:
				ReadAndWrite<ECS::AliveTime, ECS::LifeLimit, ECS::ColorLarp, ECS::Color> readWrite_;
				using DataBase = DataBase<decltype(readWrite_)>;

				void Execute(const World *const, const float);

			};

			class AddGravity :public IJobEntity {
			public:
				ReadAndWrite<ECS::AccelerationComp, ECS::GravityComp> readWrite_;
				using DataBase = DataBase<decltype(readWrite_)>;

				void Execute(const World *const, const float);

			};
			class AirResistanceSystem :public IJobEntity {
			public:
				ReadAndWrite<ECS::AccelerationComp, const ECS::VelocityComp, const ECS::AirResistance> readWrite_;
				using DataBase = DataBase<decltype(readWrite_)>;

				void Execute(const World *const, const float);

			};
			class MovePosition :public IJobEntity {
			public:
				ReadAndWrite<ECS::PositionComp, ECS::VelocityComp, ECS::AccelerationComp> readWrite_;
				using DataBase = DataBase<decltype(readWrite_)>;

				void Execute(const World *const, const float);
			};

			class EnemyMove :public IJobEntity {
			public:
				ReadAndWrite<ECS::EnemyTag, ECS::PositionComp, ECS::QuaternionRotComp> readWrite_;
				using DataBase = DataBase<decltype(readWrite_)>;

				static Vector3 playerPos_;

				void Execute(const World *const, const float);
			};

			class EnemyAttack :public IJobEntity {
			public:
				ReadAndWrite<ECS::PlayerTag, ECS::PositionComp, ECS::HealthComp, ECS::SphereCollisionComp, ECS::AccelerationComp> readWrite_;
				using DataBase = DataBase<decltype(readWrite_)>;

				void Execute(const World *const, const float);
			};

			class FallCollision :public IJobEntity {
			public:
				ReadAndWrite<ECS::SphereCollisionComp, ECS::PositionComp, ECS::VelocityComp> readWrite_;
				using DataBase = DataBase<decltype(readWrite_)>;

				static const Ground *ground_;

				void Execute(const World *const, const float);
			};

			class WeaponCollision :public IJobEntity {
			public:
				ReadAndWrite<ECS::EnemyTag, ECS::PositionComp, ECS::SphereCollisionComp, ECS::HealthComp> readWrite_;
				using DataBase = DataBase<decltype(readWrite_)>;

				void Execute(const World *const, const float);
			};

			class PlayerMove :public IJobEntity {
			public:
				ReadAndWrite<ECS::PositionComp, ECS::QuaternionRotComp, ECS::AccelerationComp, ECS::InputFlagComp, ECS::ModelAnimator, ECS::IsLanding, ECS::AttackStatus, ECS::AttackCooltime> readWrite_;
				using DataBase = DataBase<decltype(readWrite_)>;

				void Execute(const World *const, const float);
			};


			class PlayerAttack :public IJobEntity {
			public:
				ReadAndWrite<ECS::PositionComp, ECS::QuaternionRotComp, ECS::AttackStatus, ECS::AttackCooltime, ECS::AttackCollisionComp, ECS::CursorComp, ECS::ModelAnimator, ECS::EntityState, ECS::Experience> readWrite_;
				using DataBase = DataBase<decltype(readWrite_)>;

				static Model *attackModel_;

				void Execute(const World *const, const float);
			};

			class ModelDrawer :public IJobEntity {
			public:
				static constexpr bool kIsSingleThread_ = true;
				ReadAndWrite<ECS::TransformMatComp, ECS::ModelComp> readWrite_;
				using DataBase = DataBase<decltype(readWrite_)>;
				using Exclusions = Exclusions<ECS::SkinModel, ECS::UnRender>;

				void Execute(const World *const, const float);
			};

			class SkinModelDrawer :public IJobEntity {
			public:
				static constexpr bool kIsSingleThread_ = true;
				ReadAndWrite<ECS::TransformMatComp, ECS::ModelComp, ECS::SkinModel> readWrite_;
				using DataBase = DataBase<decltype(readWrite_)>;

				void Execute(const World *const, const float);
			};

			class SlideFollowCameraUpdate :public IJobEntity {
			public:
				ReadAndWrite<ECS::FollowCamera, ECS::PositionComp> readWrite_;
				using DataBase = DataBase<decltype(readWrite_)>;

				void Execute(const World *const, const float);
			};

			class CalcTransMatrix : public IJobEntity {
			public:
				OnlyRead<ECS::ScaleComp, ECS::QuaternionRotComp, ECS::PositionComp> onlyRead_;
				OnlyWrite<ECS::TransformMatComp> onlyWrite_;
				using DataBase = DataBase<decltype(onlyRead_), decltype(onlyWrite_)>;

				void Execute(const World *const, const float);
			};

			class CalcEulerTransMatrix :public IJobEntity {
			public:
				OnlyRead<ECS::ScaleComp, ECS::RotateComp, ECS::PositionComp> onlyRead_;
				OnlyWrite<ECS::TransformMatComp> onlyWrite_;
				using DataBase = DataBase<decltype(onlyRead_), decltype(onlyWrite_)>;

				void Execute(const World *const, const float);
			};

			class DrawHelthBar :public IJobEntity {
			public:
				ReadAndWrite<ECS::PlayerTag, ECS::HealthComp> readWrite_;
				using DataBase = DataBase<decltype(readWrite_)>;

				static HealthBar *healthBar_;

				void Execute(const World *const, const float);
			};

			class DrawEnemyHelthBar :public IJobEntity {
			public:
				static constexpr bool kIsSingleThread_ = true;
				ReadAndWrite<ECS::EnemyTag, ECS::HealthComp, ECS::PositionComp, ECS::HealthBarComp> readWrite_;
				using DataBase = DataBase<decltype(readWrite_)>;

				inline static constexpr uint32_t kDrawCount_ = 50u;

				static std::array<std::unique_ptr<HealthBar>, kDrawCount_> *healthBar_;
				static uint32_t drawCount_;

				static void Init(std::array<std::unique_ptr<HealthBar>, kDrawCount_> *healthBar);

				void Execute(const World *const, const float);
			};

			class CursorDrawer :public IJobEntity {
			public:
				ReadAndWrite<ECS::PlayerTag, ECS::PositionComp, ECS::QuaternionRotComp, ECS::CursorComp> readWrite_;
				using DataBase = DataBase<decltype(readWrite_)>;

				void Execute(const World *const, const float);
			};

			class ExpGaugeDrawer :public IJobEntity {
			public:
				ReadAndWrite<ECS::Experience> readWrite_;
				using DataBase = DataBase<decltype(readWrite_)>;

				static uint32_t prevLevel_;
				static SoLib::DeltaTimer levelUpTimer_;
				static LevelUP *levelUp_;
				static HealthBar *expBar_;

				void Execute(const World *const, const float);
			};

			class CalcParentTransform :public IJobEntity {
			public:
				ReadAndWrite<ECS::TransformMatComp, ECS::Parent> readWrite_;
				using DataBase = DataBase<decltype(readWrite_)>;

				void Execute(const World *const, const float);
			};

		}
	}
}