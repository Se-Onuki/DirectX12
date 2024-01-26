#pragma once
#include "../Collision/Collision.h"
#include "../../Utils/Math/Vector2.h"
#include "../../Utils/Math/Matrix4x4.h"
#include "../../Engine/DirectBase/Model/Model.h"

class Ground {
public:

	void Init();

	float hight_;
	Vector2 radius_;

	Model *model_;

	Matrix4x4 CalcMatrix() const;

};