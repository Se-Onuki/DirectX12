#pragma once
#include "SceneManager.h"

#include "../Engine/DirectBase/3D/DirectionLight.h"
#include "../Engine/DirectBase/2D/Sprite.h"
#include "../Engine/DirectBase/Render/CameraAnimations/CameraManager.h"

#include "../Engine/DirectBase/Model/Model.h"
#include "../Utils/Math/Transform.h"

#include "../Engine/DirectBase/Input/Input.h"
#include "../Engine/DirectBase/Base/Audio.h"
#include "../ECS/World/World.hpp"
#include "../ECS/Entity/EntityManager.hpp"
#include "../ECS/MultiArray.h"

#include "../Utils/Graphics/Color.h"
#include "../Utils/IO/CSV.h"
#include "../../Header/Object/Particle.h"
#include "../Utils/Containers/ConstVector.h"

class GameScene : public IScene {
public:
	GameScene();
	~GameScene();

	void OnEnter() override;
	void OnExit() override;

	void Update() override;
	void Draw() override;

private:

	Input *input_ = nullptr;
	Audio *audio_ = nullptr;
	CameraManager *cameraManager_ = nullptr;

	std::unique_ptr<DirectionLight> light_ = nullptr;

	StructuredBuffer<Particle::ParticleData> particleArray_{ 1024u };

	Model *model_ = nullptr;

	std::unique_ptr<World> world_ = nullptr;
	ECS::EntityManager *entityManager_;

	ECS::Prefab prefab;

};