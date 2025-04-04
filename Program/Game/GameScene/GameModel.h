/// @file GameModel.h
/// @brief ゲームで使用するデータの実体
/// @author ONUKI seiya
#pragma once
#include <memory>


#include "../Resource/EnemyDataTable.h"
#include "../Resource/GameScore.h"

namespace Game {

	class GameRenderer;

	class GameModel {
	public:
		friend GameRenderer;

		GameModel() = default;
		~GameModel() = default;

		void Update(float deltaTime);

	private:


		// 敵の強さのテーブル
		std::unique_ptr<EnemyTable> enemyTable_;

		// ゲームのスコアを返す
		std::unique_ptr<GameScore> gameScore_;


	};

}
