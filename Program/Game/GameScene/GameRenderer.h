/// @file GameRenderer.h
/// @brief ゲームシーンの描画クラス
/// @author ONUKI seiya

#pragma once
#include "../../Engine/ResourceObject/ResourceObjectManager.h"
#include "../../Engine/DirectBase/Base/Shader.h"

namespace Game {

	class GameModel;

	class GameRenderer {
	public:

		GameRenderer() = default;
		~GameRenderer() = default;

		/// @brief 初期化
		void Init();


		/// @brief ゲームの情報の描画
		/// @param[in] gameModel ゲームの情報
		void Render(const GameModel &gameModel);


	private:


		/// @brief シェーダーのリソースマネージャ
		SolEngine::ResourceObjectManager<Shader> *pShaderManager_ = nullptr;
	};

}