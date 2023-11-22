#pragma once
#include "Particle.h"
#include "../../Engine/DirectBase/Base/StructBuffer.h"
#include "../../Engine/DirectBase/Model/Model.h"
#include <list>

class CollisionDrawer final {

	CollisionDrawer() = default;
	CollisionDrawer(const CollisionDrawer &) = delete;
	CollisionDrawer &operator=(const CollisionDrawer &) = delete;
	~CollisionDrawer() = default;
public:

	static auto *const GetInstance() {
		static CollisionDrawer instance{};
		return &instance;
	}

	void Init();




private:

	// std::list<ParticleList> particleList_;

	uint32_t back_;

	uint32_t size_;

	StructuredBuffer<Particle> particleArray_{ 256u };

};