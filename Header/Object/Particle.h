#pragma once
#include "../../Utils/Math/Transform.h"
#include "../../Utils/Math/Vector4.h"

struct Particle {
	struct ParticleData {
		TransformMatrix transform;
		Vector4 color;
	};
	BaseTransform transform_;
	Vector4 color_;

	using map_struct = ParticleData;
	Particle &operator=(map_struct *const target);
};
