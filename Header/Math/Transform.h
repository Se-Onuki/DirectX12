#pragma once
#include "Vector3.h"

struct Transform
{
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;

	_NODISCARD Matrix4x4 Affine() const;
};