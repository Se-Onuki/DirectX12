#pragma once
#include <optional>
#include "AnimationList.h"
#include "AnimationParameters.h"

/// <summary>
/// プレイヤーのアニメーションマネージャー
/// </summary>
class AnimationManager
{
public: // サブクラス

	/// <summary>
	/// 行動列挙子
	/// </summary>
	enum Behavior {
		kIdle,
		kMove,
		kJump
	};

public: // メンバ関数

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新関数
	/// </summary>
	void Update();

public: // アクセッサ等

	/// <summary>
	/// エンティティセッター
	/// </summary>
	/// <param name="entity">アニメーションさせるエンティティ</param>
	void SetEntity(Entity* entity) { entity_ = entity; }

	/// <summary>
	/// 次のアニメーションセッター
	/// </summary>
	/// <param name="next">次のアニメーション実体</param>
	void SetNextAnimation(BaseAnimation* next) { nextAnimation_ = next; }

private: // メンバ変数

	// アニメーション数値クラス
	std::unique_ptr<AnimationParameters> animParameters_ = nullptr;

	// アニメーションさせるエンティティ
	Entity* entity_ = nullptr;

	// 現在のアニメーション
	BaseAnimation* currentAnimation_ = nullptr;

	// 次のアニメーション
	BaseAnimation* nextAnimation_ = nullptr;

};

