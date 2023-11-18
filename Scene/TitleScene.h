#pragma once
#include "SceneManager.h"

#include "../Engine/DirectBase/3D/DirectionLight.h"
#include "../Engine/DirectBase/2D/Sprite.h"

#include "../Engine/DirectBase/Input/Input.h"
#include "../Engine/DirectBase/Base/Audio.h"

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

	std::array<Vector3, 2u> from_{
		Vector3{1.f,0.7f,0.5f}.Nomalize(),
		Vector3{-0.6f,0.9f,0.2f}.Nomalize(),
	};

	std::array<Vector3, 2u> to_{
		-from_[0u],
		Vector3{0.4f,0.7f,-0.5f},
	};

	std::array<Matrix4x4, 3u> rotateMat_;

	std::unique_ptr<DirectionLight> light_ = nullptr;
};