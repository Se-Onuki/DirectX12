#include "GameRenderer.h"


#include "GameModel.h"

void Game::GameRenderer::Init()
{
	pShaderManager_ = SolEngine::ResourceObjectManager<Shader>::GetInstance();
}

void Game::GameRenderer::Render(const GameModel &)
{



}
