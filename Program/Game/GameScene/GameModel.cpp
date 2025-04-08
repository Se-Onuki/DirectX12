#include "GameModel.h"

namespace Game {
	void GameModel::Init()
	{
		controllerUI_ = std::make_unique<Game::ControllerUI>();
	}
	void GameModel::Update(float deltaTime)
	{
		controllerUI_->Update(deltaTime);
	}

}