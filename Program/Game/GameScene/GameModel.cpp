#include "GameModel.h"

namespace Game {

	void GameModel::Update(float deltaTime)
	{
		controllerUI_->Update(deltaTime);
	}

}