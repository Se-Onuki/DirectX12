#pragma once
#include <optional>
#include "../Entity.h"
#include "../../../Engine/DirectBase/File/VariantItem.h"

#include "PlayerAnimations/AnimationManager.h"

/// <summary>
/// プレイヤーアニメーションコンポーネント
/// </summary>
class PlayerAnimComp : public IComponent
{
public: // メンバ関数

	// 基底コンポーネントのコンストラクタを使用
	using IComponent::IComponent;
	// デストラクタ
	~PlayerAnimComp() = default;

	/// <summary>
	/// 初期化関数
	/// </summary>
	void Init() override;

	/// <summary>
	/// 更新関数
	/// </summary>
	void Update() override;

public: // アクセッサ等

	/// <summary>
	/// アニメーションマネージャーゲッター
	/// </summary>
	/// <returns>アニメーションマネージャー</returns>
	AnimationManager *GetAnimManager() { return animManager_.get(); }

	/// <summary>
	/// プレイヤーの輪っか座標ゲッター
	/// </summary>
	/// <returns>プレイヤーの輪っか座標</returns>
	const BaseTransform* GetLingTransform() { return &LingTransform_; }

private: // メンバ変数
	// モデルのトランスフォーム
	BaseTransform bodyTransform_; // 体のトランスフォーム
	BaseTransform eyeTransform_; // 瞳のトランスフォーム
	BaseTransform LingTransform_; // 輪っかトランスフォーム
	BaseTransform ArmTransform_L_; // 左腕トランスフォーム
	BaseTransform ArmTransform_R_; // 右腕トランスフォーム
	BaseTransform FootTransform_L_; // 左足トランスフォーム
	BaseTransform FootTransform_R_; // 右足トランスフォーム

	// アニメーションマネージャー
	std::unique_ptr<AnimationManager> animManager_ = nullptr;
};

