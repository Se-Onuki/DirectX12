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
#include "../ECS/Viewer.h"

#include "../Utils/Graphics/Color.h"
#include "../Utils/IO/CSV.h"
#include "../../Header/Object/Particle.h"

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

	ArrayBuffer<Particle::ParticleData> particleArray_{ 1024u };


	Model *model_ = nullptr;

	World *world_ = nullptr;

	std::unique_ptr<ECS::MultiArray> mArray_;
	std::unique_ptr<ECS::MultiArray> emitterArray_;

	SoLib::Color::RGB4 rgbFrom_ = Vector4::one;
	SoLib::Color::RGB4 rgbTo_ = Vector4::one;
	float t_{};

	//std::unique_ptr<ECS::Viewer<ECS::Identifier>> mViewer_;


};