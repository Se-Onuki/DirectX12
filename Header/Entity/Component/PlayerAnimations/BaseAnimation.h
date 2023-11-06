#pragma once
#include "../../Entity.h"
#include "../ModelComp.h"
#include "AnimEasing.h"
#include "../../../../Engine/DirectBase/File/GlobalVariables.h"
#include "PlayerBone.h"

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
	/// <param name="transitionTime">遷移秒数</param>
	virtual void Initialize(float transitionTime = 0.0f);

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
	/// <summary>
	/// 終了状態セッター
	/// </summary>
	/// <param name="isEnd">終了状態</param>
	void SetIsEnd(bool isEnd) { isEnd_ = isEnd; }

	/// <summary>
	/// 引数で指定したボーンの値を取得するゲッター
	/// </summary>
	/// <param name="groupName">取得するボーンのグループ名</param>
	/// <param name="boneName">取得するボーン名</param>
	/// <returns>目標ボーン</returns>
	PlayerBone::Bone GetTargetBone(std::string groupName, std::string boneName);

	/// <summary>
	/// 引数で指定したボーンの値を取得するゲッター
	/// </summary>
	/// <returns>プレイヤーボーン</returns>
	PlayerBone::Bone GetPlayerBone();

protected: // 継承先メンバ変数

	// グローバル変数クラス
	GlobalVariables* globalVariables_ = nullptr;

	// アニメーションさせるエンティティ
	Entity* entity_ = nullptr;

	// 現在ボーン
	PlayerBone bone_;

	// アニメーション用ボーン
	PlayerBone::Bone startBone_; // 始端
	PlayerBone::Bone endBone_;	 // 終端

	// アニメーション演出用t
	float animT_;
	// アニメーション演出時間秒数
	float animationTime_;

	// 遷移開始直前のボーン情報を取得
	PlayerBone::Bone prevBone_;
	// 遷移中トリガー
	bool isTransitioning_;
	// 遷移秒数
	float transitionTime_;

	// 終了トリガー
	bool isEnd_;

};

