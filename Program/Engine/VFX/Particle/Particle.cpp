#include "Particle.h"

namespace SolEngine::VFX {

	void ParticleEmitter::Init()
	{

	}

	void ParticleEmitter::Start()
	{
		timer_.Start();
	}

	void ParticleEmitter::Update(float deltaTime)
	{
		timer_.Update(deltaTime);
		EmitUpdate(deltaTime);
	}

	void ParticleEmitter::EmitUpdate(float deltaTime) const
	{

	}

	void ParticleEmitter::Generate(ECS::World *const world) const
	{
		world->CreateEntity(particleBase_);
	}

	void ParticleManager::AddEmitter(std::unique_ptr<ParticleEmitter> particleEmitter)
	{
		particleBuffer_.push_back(std::move(particleEmitter));
	}

	void ParticleManager::Update(float deltaTime)
	{

	}

	std::unique_ptr<ParticleManager> SolEngine::VFX::ParticleManager::Generate()
	{
		auto result = std::make_unique<ParticleManager>();

		result->world_ = std::make_unique<ECS::World>();

		return std::move(result);
	}

}