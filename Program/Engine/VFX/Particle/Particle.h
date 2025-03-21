/// @file Particle.h
/// @brief Particleの実装
/// @author ONUKI seiya
#pragma once
#include <cstdint>
#include "../../Utils/Math/Math.hpp"

#include "../Engine/DirectBase/Base/EngineObject.h"
#include "../Engine/DirectBase/Base/StructBuffer.h"

#include "../Engine/ECS/World/NewWorld.h"

namespace SolEngine::VFX
{

	class IParticle {
	public:


	};

	class TestParticle : public IParticle {
	public:


	};

	class ParticleEmitter {
	public:

		void Init();

		void Start();

		void Update(float deltaTime);

		void EmitUpdate(float deltaTime) const;

	private:

		void Generate(uint32_t count) const;

		SoLib::Time::DeltaTimer timer_;

		uint32_t particleSpawnOfTime_ = 0;

		SolEngine::ModelData modelData_;

	};

	class ParticleManager : EngineObject {
	public:

		ParticleManager() = default;

		void AddEmitter(std::unique_ptr<ParticleEmitter> particleEmitter);

		void Update(float deltaTime);

		static std::unique_ptr<ParticleManager> Generate();

	private:



		std::list<std::unique_ptr<ParticleEmitter>> particleBuffer_;
	};

	class ParticleRender : EngineObject {
	public:

		void ExecuteDraw(ParticleManager *);

	private:

	};



}