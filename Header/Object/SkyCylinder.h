#pragma once

#include "../Math/Transform.h"

class ViewProjection;
class Model;

class SkyCylinder {
	Model *model_ = nullptr;
	Transform transform_;

	static float radius_;

public:
	SkyCylinder() = default;
	~SkyCylinder() = default;

	void Init();
	void Update();

	void Draw(const ViewProjection &Vp) const;

	void SetRadius(const float value);
	static float GetRadius() { return radius_; }
};
