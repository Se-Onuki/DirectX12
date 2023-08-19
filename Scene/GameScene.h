#pragma once
#include "SceneManager.h"
#include "../DirectBase/3D/ViewProjection/ViewProjection.h"
#include "../DirectBase/3D/DirectionLight.h"
#include "../DirectBase/2D/Sprite.h"
#include "../Header/Model/Object.h"
#include "../DirectBase/Base/Audio.h"

class Player;
class FollowCamera;
class Ground;
class Input;

class GameScene : public IScene {
public:
	GameScene();
	~GameScene();

	void OnEnter() override;
	void OnExit() override;

	void Update() override;
	void Draw() override;
public:

private:
	Input *input_ = nullptr;
	Audio *audio_ = nullptr;

	std::unique_ptr<DirectionLight> light_ = nullptr;

	std::unique_ptr<Player> player_ = nullptr;
	std::unique_ptr<FollowCamera> followCamera_ = nullptr;
	std::unique_ptr<Ground> ground_ = nullptr;

	ViewProjection viewProjection_{};



};