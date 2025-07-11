#include "GameRenderer.h"


#include "GameModel.h"

void Game::GameSceneRenderer::Init()
{
	pShaderManager_ = SolEngine::ResourceObjectManager<Shader>::GetInstance();
}

void Game::GameSceneRenderer::Render(const GameSceneModel &)
{



}
