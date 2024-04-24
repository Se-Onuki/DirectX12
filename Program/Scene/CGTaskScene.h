#pragma once
#include "SceneManager.h"
#include <memory>
#include "../Engine/DirectBase/3D/DirectionLight.h"
#include "../Header/Entity/Entity.h"
#include "../Engine/DirectBase/PostEffect/OffScreenRendering.h"

class CGTaskScene : public IScene {
public:
	void OnEnter() override;
	void OnExit() override;

	void Update() override;
	void Draw() override;

private:
	std::unique_ptr<DirectionLight> light_ = nullptr;

	Model *model_;

	Transform transform_;

	PostEffect::OffScreenRenderer *offScreen_ = nullptr;

	ModelAnimation::AnimationPlayer animation_;

};