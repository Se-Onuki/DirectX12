#pragma once

namespace ECS {

	enum class SimulationSystemGroup {
		BeginSimulationEntityCommandBufferSystem,
		TransformSystemGroup,
		EndFrameParentSystem,
		CopyTransformFromGameObjectSystem,
		EndFrameTRSToLocalToWorldSystem,
		EndFrameTRSToLocalToParentSystem,
		EndFrameLocalToParentSystem,
		CopyTransformToGameObjectSystem,
		LateSimulationSystemGroup,
		EndSimulationEntityCommandBufferSystem,
	};

	class ISystem {
	public:
		SimulationSystemGroup systemGroup_;
		virtual ~ISystem() = 0;
		virtual void OnUpdate(const float deltaTime) = 0;
		//virtual void PreLateUpdate() = 0;
	};

	class IJobSystem {
	public:
		virtual ~IJobSystem() = 0;

		virtual void OnUpdate(const float deltaTime) = 0;
	};

}