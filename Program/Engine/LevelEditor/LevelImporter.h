/// @file LevelImporter.h
/// @brief レベルの読み込みクラス
/// @author ONUKI seiya
#pragma once
#include "LevelData.h"
#include "../../Engine/ECS/World/NewWorld.h"

namespace SolEngine {
	class LevelImporter {
	public:

		/// @brief レベルを読み込む
		/// @param[in] levelData レベルデータ
		/// @param[out] world ワールド
		bool Import(const ResourceObjectManager<LevelData>::Handle levelData, ECS::World *const world) const;


	private:
		/// @brief 再帰的にロードを行う
		/// @param[in] objectDataList 読み込み元のデータ
		/// @param[out] world 書き込み先のワールド
		/// @param[out] parent 親となるオブジェクト
		void RecursiveLoad(const std::list<LevelData::ObjectData> &objectDataList, ECS::World *const world, ECS::EntityClass *parent = nullptr) const;

	};
}