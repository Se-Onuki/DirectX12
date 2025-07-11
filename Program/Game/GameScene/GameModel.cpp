#include "GameModel.h"

namespace Game {
	void GameSceneModel::Init()
	{
		controllerUI_ = std::make_unique<Game::ControllerUI>();
	}
	void GameSceneModel::Update(float deltaTime)
	{
		controllerUI_->Update(deltaTime);
	}

}