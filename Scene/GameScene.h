#pragma once
#include "SceneManager.h"
#include "../DirectBase/3D/ViewProjection/ViewProjection.h"
#include "../DirectBase/3D/DirectionLight.h"
#include "../DirectBase/2D/Sprite.h"

#include "../Header/Entity/Object.h"

class Player;
class PlayerBullet;

class Enemy;

class CollisionManager;

class FollowCamera;
class Ground;

class Input;
class Audio;

class Targeting;

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
	void AddEnemy(Enemy *newEnemy);

	void AddPlayer();

	void PopEnemy();

	void EndGame();

private:
	Input *input_ = nullptr;
	Audio *audio_ = nullptr;

	std::unique_ptr<DirectionLight> light_ = nullptr;

	std::unique_ptr<Player> player_ = nullptr;
	std::list<std::unique_ptr<PlayerBullet>> pBulletList_;
	std::list<std::unique_ptr<Enemy>> enemyList_;

	std::unique_ptr<FollowCamera> followCamera_ = nullptr;
	std::unique_ptr<Ground> ground_ = nullptr;

	ViewProjection viewProjection_{};

	CollisionManager *collisionManager_ = nullptr;

	Targeting *targeting_ = nullptr;

};