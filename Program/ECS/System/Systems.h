#pragma once
#include "ISystem.h"
#include "../Component/Component.hpp"
#include "../../Header/Object/Ground.h"
#include "../../Engine/DirectBase/Base/Audio.h"
#include "../../Engine/DirectBase/Model/Model.h"

namespace ECS {
	namespace System {

		class CheckAliveTime : public ISystem {

			bool IsConstSystem() const override { return false; }
			void OnUpdate(::World *world, const float deltaTime) override;

		};

		class AddAliveTime : public ISystem {

			bool IsConstSystem() const override { return false; }
			void OnUpdate(::World *world, const float deltaTime) override;

		};

		class AnimateUpdate : public ISystem {

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

		class FollowCameraUpdate : public ISystem {
		public:

			bool IsConstSystem() const override { return false; }
			void OnUpdate(::World *world, const float deltaTime) override;
		};

		class MakeTransMatrix : public ISystem {
		public:

			bool IsConstSystem() const override { return false; }
			void OnUpdate(::World *world, const float deltaTime) override;
		};


	}
}