#pragma once
#include "SceneManager.h"

#include "../Engine/DirectBase/3D/DirectionLight.h"
#include "../Engine/DirectBase/2D/Sprite.h"
#include "../Engine/DirectBase/Render/CameraAnimations/CameraManager.h"

#include "../Engine/DirectBase/Model/Model.h"
#include "../Utils/Math/Transform.h"

#include "../Engine/DirectBase/Input/Input.h"
#include "../Engine/DirectBase/Base/Audio.h"
#include "../Header/Object/LevelElementManager.h"
#include "../Header/Object/Particle.h"

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

	//	Model *model_ = nullptr;
	std::unique_ptr<Sprite> sprite_ = nullptr;
	//Transform transform_;
	Camera3D camera_;

	std::unique_ptr<DirectionLight> light_ = nullptr;

	std::unique_ptr<Entity> player_;
	std::unique_ptr<Entity> followCamera_ = nullptr;
	LevelElementManager *levelManager{};

	//std::array<const Camera3D *, 2u> cameraList_;
	//std::array<const Camera3D *, 2u>::iterator cameraTarget_ = cameraList_.begin();

	//std::unique_ptr<Entity> playerAnim_;

	//std::array<BaseTransform<false>, 5u> transformArray_;
	//std::array<CBuffer<Vector4, false>, 5u> colorArray_;

	//StructuredBuffer<Particle> instanceTransform_{ 5u };
};