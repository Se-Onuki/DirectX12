#pragma once

#include "SceneManager.h"
#include "../DirectBase/3D/DirectionLight.h"
#include "../DirectBase/2D/Sprite.h"

class Input;
class Audio;

class GameOverScene : public IScene {
public:
	GameOverScene();
	~GameOverScene();

	void OnEnter() override;
	void OnExit() override;

	void Update() override;
	void Draw() override;
public:

private:

	Input *input_ = nullptr;
	Audio *audio_ = nullptr;

	std::unique_ptr<Sprite> backGround_ = nullptr;
	std::unique_ptr<Sprite> gameOverSprite_ = nullptr;
};