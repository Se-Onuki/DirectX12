#pragma once

namespace ECS {

	class ISystem {
	public:
		virtual ~ISystem() = 0;
		virtual void OnUpdate(float deltaTime) = 0;
	};

	class IJobSystem {
	public:
		virtual ~IJobSystem() = 0;

		virtual void OnUpdate(float deltaTime) = 0;
	};

}