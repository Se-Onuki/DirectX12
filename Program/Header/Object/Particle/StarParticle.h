#pragma once
#include "ParticleManager.h"
#include "../../Engine/DirectBase/File/GlobalVariables.h"
#include "../../Entity/Entity.h"

/// <summary>
/// プレイヤーの後ろ方向に向かって星を放出するパーティクル
/// </summary>
class StarParticle : public IParticle
{
public: // 静的なメンバ関数
	
	/// <summary>
	/// 調整項目クラスに値を追加する関数
	/// </summary>
	static void AddItem();
	/// <summary>
	/// 調整項目クラスに値を追加する関数
	/// </summary>
	static void SetItem();
	/// <summary>
	/// 調整項目クラスに値を追加する関数
	/// </summary>
	static void ApplyItem();

	/// <summary>
	/// ImGui表示関数
	/// </summary>
	static void DisplayImGui();

public: // メンバ関数

	// 基底クラスのコンストラクタを使用
	using IParticle::IParticle;

	/// <summary>
	/// 初期化関数
	/// </summary>
	void Init() override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update(float deltaTime) override;

public: // 静的なメンバ変数

	// １フレームごとの回転量
	static float rotationAmount_;

	// 粒子最小サイズ
	static float minSize_;
	// 粒子最大サイズ
	static float maxSize_;

	// 粒子が表示されるまでの秒数
	static float dispalyParticleTime_;

private: // メンバ変数

	// 移動ベクトル格納用
	Vector3 move_;

	// パーティクル表示用タイマー
	SoLib::DeltaTimer diplayParticleTimer_;

	// 開始時サイズ格納用
	Vector3 startScale_ = Vector3::zero;

};

