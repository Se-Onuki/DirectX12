#pragma once
#include "../Math/Transform.h"

class ViewProjection;
class Model;

class Ground {
	Model *model_ = nullptr;
	Transform transform_;

	float stageRadius_ = 200.f;

public:
	Ground() = default;
	~Ground() = default;

	void Init();
	void Update();

	void Draw(const ViewProjection &Vp) const;

	void SetStageRadius(const float value);
	float GetStageRadius() const { return stageRadius_; }
};
