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
		kJumpStart,
		kHovering,
		kLand,
		kRotateStart,
		kRotating,
		kRotateEnd
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
	/// <param name="next">次にどのアニメーションを再生するか</param>
	/// <param name="isLoop">ループさせるか</param>
	/// <param name="type">遷移イージングのタイプ</param>
	/// <param name="transitionTime">遷移の際の時間</param>
	void SetNextAnimation(Behavior next, bool isLoop, AnimEasing::EasingType type = AnimEasing::kLinear, float transitionTime = 0);

private: // メンバ変数

	// アニメーション数値クラス
	std::unique_ptr<AnimationParameters> animParameters_ = nullptr;

	// アニメーションさせるエンティティ
	Entity* entity_ = nullptr;

	// 現在のアニメーション
	BaseAnimation* currentAnimation_ = nullptr;

	// 次のアニメーション
	BaseAnimation* nextAnimation_ = nullptr;

#ifdef _DEBUG // ImGUiデバッグ用

	// ループトリガー
	bool imGuiIsLoop_ = false;
	// 遷移秒数設定用
	float imGuiTransitionTime_ = 0.0f;
	// 遷移のイージングタイプ
	int imGuiTransitonType_ = AnimEasing::kLinear;
	// 次のアニメーション
	int imGuiNextbehavior_ = kIdle;

	// アニメーションキーコピー用
	AnimationKeys imGuiCopyKeys_;

#endif // _DEBUG // ImGUiデバッグ用


};

