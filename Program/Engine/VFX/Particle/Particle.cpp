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
		if (deltaTime <= 0) { return; }
		Generate(static_cast<uint32_t>(particleSpawnOfTime_ / deltaTime));
	}

	void ParticleEmitter::Generate([[maybe_unused]] uint32_t count) const
	{
	}

	void ParticleManager::AddEmitter(std::unique_ptr<ParticleEmitter> particleEmitter)
	{
		particleBuffer_.push_back(std::move(particleEmitter));
	}

	void ParticleManager::Update(float deltaTime)
	{
		std::for_each(particleBuffer_.begin(), particleBuffer_.end(), [deltaTime](auto &buffer) {buffer->Update(deltaTime); });
	}

	std::unique_ptr<ParticleManager> SolEngine::VFX::ParticleManager::Generate()
	{
		auto result = std::make_unique<ParticleManager>();


		return std::move(result);
	}

}