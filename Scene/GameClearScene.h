#pragma once

#include "SceneManager.h"
#include "../DirectBase/3D/DirectionLight.h"
#include "../DirectBase/2D/Sprite.h"

class Input;
class Audio;

class GameClearScene : public IScene {
public:
	GameClearScene();
	~GameClearScene();

	void OnEnter() override;
	void OnExit() override;

	void Update() override;
	void Draw() override;
public:

private:

	Input *input_ = nullptr;
	Audio *audio_ = nullptr;

	std::unique_ptr<Sprite> backGround_ = nullptr;
	std::unique_ptr<Sprite> gameClearSprite_ = nullptr;
};