#pragma once
#include "SceneManager.h"
#include <memory>
#include "../Engine/DirectBase/3D/DirectionLight.h"
#include "../Header/Entity/Entity.h"

class CGTaskScene : public IScene {
public:
	void OnEnter() override;
	void OnExit() override;

	void Update() override;
	void Draw() override;

private:
	std::unique_ptr<DirectionLight> light_ = nullptr;

	std::unique_ptr<GameObject> gameObject_;

};