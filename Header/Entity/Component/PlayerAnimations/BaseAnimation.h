#pragma once
#include "../../Entity.h"
#include "../ModelComp.h"
#include "AnimEasing.h"
#include "../../../../Engine/DirectBase/File/GlobalVariables.h"

/// <summary>
/// アニメーション基底クラス
/// </summary>
class BaseAnimation
{
public: // メンバ関数

	// コンストラクタ
	BaseAnimation() = default;
	// デストラクタ
	~BaseAnimation() = default;

	/// <summary>
	/// 初期化関数
	/// </summary>
	virtual void Initialize();

	/// <summary>
	/// 更新関数
	/// </summary>
	/// <param name="deltaTime">経過秒数</param>
	virtual void Update(float deltaTime);

public: // アクセッサ等

	/// <summary>
	/// アニメーションさせるエンティティをセット
	/// </summary>
	/// <param name="entity">エンティティ</param>
	void SetEntity(Entity* entity) { entity_ = entity; }

	/// <summary>
	/// 終了状態ゲッター
	/// </summary>
	/// <returns>終了状態</returns>
	bool GetIsEnd() { return isEnd_; }
protected: // 継承先メンバ変数

	// グローバル変数クラス
	GlobalVariables* globalVariables_ = nullptr;

	// アニメーションさせるエンティティ
	Entity* entity_ = nullptr;

	// アニメーション演出用t
	float animT_;
	// アニメーション演出時間秒数
	float animationTime_;

	// 終了トリガー
	bool isEnd_;

};

