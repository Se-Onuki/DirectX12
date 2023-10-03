#pragma once
#include "SceneManager.h"

#include "../Engine/DirectBase/3D/DirectionLight.h"
#include "../Engine/DirectBase/2D/Sprite.h"
#include "../Engine/DirectBase/Render/Camera.h"

#include "../Engine/DirectBase/Model/Model.h"
#include "../Utils/Math/Transform.h"

#include "../Engine/DirectBase/Input/Input.h"
#include "../Engine/DirectBase/Base/Audio.h"

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

	Model *model_ = nullptr;
	std::unique_ptr<Sprite> sprite_ = nullptr;
	Transform transform_;
	Camera<Render::CameraType::Projecction> camera_;

	std::unique_ptr<DirectionLight> light_ = nullptr;


	Microsoft::WRL::ComPtr<ID3D12Resource> instancingData_ = nullptr;
};