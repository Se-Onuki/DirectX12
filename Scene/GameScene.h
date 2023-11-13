#pragma once
#include "SceneManager.h"

#include "../Engine/DirectBase/3D/DirectionLight.h"
#include "../Engine/DirectBase/2D/Sprite.h"
#include "../Engine/DirectBase/Render/Camera.h"

#include "../Engine/DirectBase/Model/Model.h"
#include "../Utils/Math/Transform.h"

#include "../Engine/DirectBase/Input/Input.h"
#include "../Engine/DirectBase/Base/Audio.h"
#include "../Header/Object/Particle.h"


#include "../Header/Object/Player.h"
#include "../Header/Object/FollowCamera.h"
#include "../Header/Object/Skydome.h"
#include "../Header/Object/Platform.h"
#include "../Header/Object/Enemy.h"

class GameScene : public IScene {
public:
	GameScene();
	~GameScene();

	void OnEnter() override;
	void OnExit() override;

	void Reset();

	void Update() override;
	void Draw() override;

private:

	Input *input_ = nullptr;
	Audio *audio_ = nullptr;

	Camera<Render::CameraType::Projecction> camera_;

	std::unique_ptr<Player> player_ = nullptr;

	std::unique_ptr<Entity> goal_ = nullptr;
	std::unique_ptr<OBB> goalCollider_ = nullptr;

	std::list<std::unique_ptr<Enemy>> enemyList_;

	std::unique_ptr<FollowCamera> followCamera_ = nullptr;
	std::unique_ptr<Skydome> skydome_ = nullptr;

	std::array<std::unique_ptr<Platform>, 4u> platform_ = { nullptr };

	std::unique_ptr<DirectionLight> light_ = nullptr;

};