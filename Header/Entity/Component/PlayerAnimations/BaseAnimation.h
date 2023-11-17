#pragma once
#include "../../Entity.h"
#include "../ModelComp.h"
#include "AnimEasing.h"
#include "../../../../Engine/DirectBase/File/GlobalVariables.h"
#include "AnimationKeys.h"
// #include "AnimationManager.h"

enum PlayerBehavior :uint32_t;

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
	/// <param name="name">読みこむパラメーター名</param>
	/// <param name="isLoop">ループするか</param>
	/// <param name="type"></param>
	/// <param name="transitionTime">遷移秒数</param>
	void Initialize(std::string name, bool isLoop, AnimEasing::EasingType type = AnimEasing::kLinear, float transitionTime = 0.0f);

	/// <summary>
	/// 更新関数
	/// </summary>
	/// <param name="deltaTime">経過秒数</param>
	void Update(float deltaTime);

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
	/// 再生状態ゲッター
	/// </summary>
	/// <returns>再生状態</returns>
	bool GetIsPlay() { return isPlaying_; }
	/// <summary>
	/// 再生状態セッター
	/// </summary>
	/// <param name="play">再生状態</param>
	void SetIsPlay(bool play) { isPlaying_ = play; }

	/// <summary>
	/// 再生中キーのゲッター
	/// </summary>
	/// <returns>再生中キー</returns>
	int GetPlayKey() { return playKey_; }

	/// <summary>
	/// 現在のアニメーション状態のゲッター
	/// </summary>
	/// <returns>現在のアニメーション状態</returns>
	const PlayerBehavior& GetBehavior() { return behavior_; }
	/// <summary>
	/// 現在のアニメーション状態セッター
	/// </summary>
	/// <param name="behavior">設定するアニメーション状態</param>
	void SetBehavior(const PlayerBehavior& behavior) { behavior_ = behavior; }

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

	/// <summary>
	/// 全てのボーンをイージングにて動作させる線形補間関数
	/// </summary>
	/// <param name="type">イージングタイプ</param>
	/// <param name="t">現在のt</param>
	/// <param name="start">開始値</param>
	/// <param name="end">終端値</param>
	/// <param name="time">時間</param>
	/// <returns>イージングされた値(float)</returns>
	PlayerBone::Bone Ease(AnimEasing::EasingType type, float t, const PlayerBone::Bone& start, const PlayerBone::Bone& end, float time);

protected: // 継承先メンバ変数

	// グローバル変数クラス
	GlobalVariables* globalVariables_ = nullptr;

	// 再生されているアニメーション
	PlayerBehavior behavior_;

	// アニメーションさせるエンティティ
	Entity* entity_ = nullptr;

	// 現在ボーン
	PlayerBone bone_;

	// アニメーションキー
	AnimationKeys animKeys_;

	// 現在再生中のキー
	int playKey_;

	// 演出用t
	float animT_;

	// 遷移開始直前のボーン情報を取得
	PlayerBone::Bone prevBone_;
	// 遷移中トリガー
	bool isTransitioning_;
	// 遷移秒数
	float transitionTime_;
	// 遷移タイプ
	AnimEasing::EasingType transitionType_;

	bool isPlaying_;

	// ループトリガー
	bool isLoop_;

	// 終了トリガー
	bool isEnd_;

};

