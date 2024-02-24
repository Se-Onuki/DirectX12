#pragma once
#include "ISystem.h"
#include "../Component/Component.hpp"
#include "../../Header/Object/Ground.h"

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

			bool IsConstSystem() const override { return false; }
			void OnUpdate(::World *world, const float deltaTime) override;
		};
	}
}