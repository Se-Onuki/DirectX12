/// @file Particle.h
/// @brief パーティクルの実装
/// @author ONUKI seiya
#pragma once
#include "../../Engine/Utils/Math/Transform.h"
#include "../../Engine/Utils/Math/Vector4.h"
#include "../../Engine/Utils/Graphics/Color.h"

struct Particle {
	struct TransMat {
		Matrix4x4 World;
	};
	struct ParticleData {
		TransMat transform;
		SoLib::Color::RGB4 color;
	};
	SoLib::BaseTransform transform_;
	SoLib::Color::RGB4 color_;

	using map_struct = ParticleData;
	Particle &operator=(map_struct *const target);
};
