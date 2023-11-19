#pragma once
#include "SceneManager.h"

#include "../Engine/DirectBase/3D/DirectionLight.h"
#include "../Engine/DirectBase/2D/Sprite.h"

#include "../Engine/DirectBase/Input/Input.h"
#include "../Engine/DirectBase/Base/Audio.h"
#include "../Utils/Math/Quaternion.h"

class TitleScene : public IScene {
public:
	TitleScene();
	~TitleScene();

	void OnEnter() override;
	void OnExit() override;

	void Update() override;
	void Draw() override;
public:

private:

	Input *input_ = nullptr;
	Audio *audio_ = nullptr;

	Quaternion rotation_ = Quaternion::AnyAxisRotation(Vector3{ 1.f,0.4f,-0.2f }.Nomalize(), 0.45f);

	Vector3 pointY_{ 2.1f,-0.9f,1.3f };

	Matrix4x4 rotateMatrix_ = rotation_.RotateMatrix();

	Vector3 rotateByQuaternion_ = Quaternion::RotateVector(pointY_, rotation_);

	Vector3 rotateByMatrix_ = pointY_ * rotation_.RotateMatrix();

	std::unique_ptr<DirectionLight> light_ = nullptr;
};