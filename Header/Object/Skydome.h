#pragma once
#include "../../Utils/Math/Transform.h"
#include "../../Engine/DirectBase/Render/Camera.h"

class Model;

class Skydome {
	Model *model_ = nullptr;
	Transform transform_;

public:
	Skydome();
	~Skydome();

	void Init();
	void Update();

	void Draw(const Camera<Render::CameraType::Projecction> &camera) const;
};
