/// @file EnemyDataTable.h
/// @brief 敵のテーブル
/// @author ONUKI seiya
#pragma once

#include "../Engine/ECS/Component/Component.hpp"
#include "../Engine/ECS/Entity/EntityList.h"

class EnemyData {
public:
	EnemyData() = default;

	int32_t spawnCount_;
	int32_t health_{};
	ECS::AttackPower power_{};
	ECS::Color color_{};
	ECS::MoveSpeed speed_{};

	auto SpawnFunc(float radDiff, float spawnRadius) const {
		return [radDiff, spawnRadius, this](const ECS::EntityList<false> &enemys) -> void
			{
				auto enemyList = enemys.View<ECS::PositionComp, ECS::HealthComp, ECS::AttackPower, ECS::Color, ECS::MoveSpeed>();
				const uint32_t spawnCount = static_cast<uint32_t>(enemyList.size());
				for (uint32_t i = 0; auto [pos, health, power, color, speed] : enemyList) {
					const float parcent = static_cast<float>(i) / spawnCount;
					pos.position_ = SoLib::EulerToDirection(SoLib::Euler{ 0.f, SoLib::Angle::Rad360 * parcent + radDiff, 0.f }) * spawnRadius;
					health = ECS::HealthComp::Create(health_);
					color = color_;
					power = power_;
					speed = speed_;

					i++;
				}


				return;
			};
	}


};

class EnemyTable {
public:
	EnemyTable() = default;

	/// @brief レベルアップに必要な時間を設定する
	/// @param time[in] 必要な秒数 
	void SetLevelUpTime(float time) { levelUpTime_ = time; }

	/// @brief 時間をもとに現在のデータを取得する
	/// @param time[in] 時間
	/// @return 取得したデータ ( 範囲外 -> nullptr )
	const EnemyData *GetEnemyDataForTime(float time) const {
		uint32_t index = static_cast<uint32_t>(time / levelUpTime_);
		if (index >= enemyDatas_.size()) { return nullptr; }
		return &enemyDatas_.at(index);
	}

	/// @brief データの追加
	/// @param enemyData[in] 追加するデータ 
	void push_back(const EnemyData &enemyData) {
		enemyDatas_.push_back(enemyData);
	}


private:

	// レベルが上昇する時間
	float levelUpTime_ = 20.f;

	// 時間のリスト
	std::vector<EnemyData> enemyDatas_;


};