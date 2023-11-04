#pragma once
#include <optional>
#include "../Entity.h"
#include "../../../Engine/DirectBase/File/VariantItem.h"

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

	void Update() override;


	//void Draw(const Camera3D& camera) const override;

private: // メンバ変数

	// モデルのトランスフォーム
	Transform bodyTransform_; // 体のトランスフォーム
	Transform eyeTransform_; // 瞳のトランスフォーム
	Transform LingTransform_; // 輪っかトランスフォーム
	Transform ArmTransform_L_; // 左腕トランスフォーム
	Transform ArmTransform_R_; // 右腕トランスフォーム
	Transform FootTransform_L_; // 左足トランスフォーム
	Transform FootTransform_R_; // 右足トランスフォーム

};

