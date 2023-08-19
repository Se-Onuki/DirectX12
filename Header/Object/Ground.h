#pragma once
#include "../Math/Transform.h"

class ViewProjection;
class Model;

class Ground {
	Model *model_ = nullptr;
	Transform transform_;

public:
	Ground() = default;
	~Ground() = default;

	void Init();
	void Update();

	void Draw(const ViewProjection &Vp) const;
};
