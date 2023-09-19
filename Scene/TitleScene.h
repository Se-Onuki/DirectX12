#pragma once
#include "SceneManager.h"
#include "../DirectBase/3D/DirectionLight.h"
#include "../DirectBase/2D/Sprite.h"

#include "../DirectBase/Input/Input.h"
#include "../DirectBase/Base/Audio.h"


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


	std::unique_ptr<DirectionLight> light_ = nullptr;
};