#pragma once
#include "../Math/Transform.h"

class ViewProjection;
class Model;

class Ground {
	Model *model_ = nullptr;
	Transform transform_;

	static float stageRadius_;

public:
	Ground() = default;
	~Ground() = default;

	void Init();
	void Update();

	void Draw(const ViewProjection &Vp) const;

	void SetStageRadius(const float value);
	static float GetStageRadius() { return stageRadius_; }
};
