#pragma once
#include "../../Entity.h"

/// <summary>
/// ゴール演出用コンポーネント
/// </summary>
class GoalAnimComp : public IComponent
{
public: // メンバ関数

	// 基底コンポーネントのコンストラクタを使用
	using IComponent::IComponent;
	// デストラクタ
	~GoalAnimComp() = default;

	/// <summary>
	/// 初期化関数
	/// </summary>
	void Init() override;

	/// <summary>
	/// 更新関数
	/// </summary>
	void Update() override;

	/// <summary>
	/// プレイヤーモデルのセッター
	/// </summary>
	/// <param name="entity">プレイヤーモデル</param>
	void SetPlayerModel(Entity* entity) { playerModel_ = entity; }

private: // メンバ変数

	// プレイヤーモデル
	Entity* playerModel_ = nullptr;

	//　再生
	bool isPlay_ = false;

};

