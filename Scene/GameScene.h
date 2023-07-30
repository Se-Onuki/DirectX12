#pragma once
#include "SceneManager.h"
#include "../DirectBase/3D/ViewProjection/ViewProjection.h"
#include "../DirectBase/3D/DirectionLight.h"
#include "../DirectBase/2D/Sprite.h"
#include "../Header/Model/Object.h"

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

	ViewProjection viewProjection;

	std::unique_ptr<DirectionLight> light = nullptr;

	std::unique_ptr<Sprite> sprite = nullptr;

	std::list<std::unique_ptr<Object>> objectArray_;

};