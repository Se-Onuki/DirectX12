#pragma once
#include "SceneManager.h"
#include "../DirectBase/3D/DirectionLight.h"

class TitleScene : public IScene {
public:
	TitleScene();
	~TitleScene();

	void OnEnter() override;
	void OnExit() override;

	void Update() override;
	void Draw() override;
public:

private:
	std::unique_ptr<DirectionLight> light_ = nullptr;


};