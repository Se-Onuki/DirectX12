#pragma once
#include "SceneManager.h"
#include "../DirectBase/3D/ViewProjection/ViewProjection.h"
#include "../DirectBase/3D/DirectionLight.h"
#include "../DirectBase/2D/Sprite.h"
#include "../DirectBase/Base/Audio.h"

#include "../Header/Entity/Object.h"

class Player;
class PlayerBullet;

class CollisionManager;

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

	void AddPlayerBullet(PlayerBullet *newBullet);

private:
	Input *input_ = nullptr;
	Audio *audio_ = nullptr;

	std::unique_ptr<DirectionLight> light_ = nullptr;

	std::unique_ptr<Player> player_ = nullptr;
	std::list<std::unique_ptr<PlayerBullet>> pBulletList_;

	std::unique_ptr<FollowCamera> followCamera_ = nullptr;
	std::unique_ptr<Ground> ground_ = nullptr;

	ViewProjection viewProjection_{};

	CollisionManager *collisionManager_ = nullptr;

};