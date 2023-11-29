#pragma once
#include "../../../../Utils/SoLib/SoLib.h"
#include "../../Entity.h"
#include "../../../../Engine/DirectBase/Model/Model.h"

// クラスの前方宣言
class CameraManager;
class GlobalVariables;
class StageSelectManager;
class ParticleEmitterManager;
class ParticleEmitter;

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

	void Draw(const Camera3D &camera) const;

	/// <summary>
	/// ImGui表示関数
	/// </summary>
	void DisplayImGui();

public: // アクセッサ等

	/// <summary>
	/// プレイヤーモデルのセッター
	/// </summary>
	/// <param name="entity">プレイヤーアニメーションを持つエンティティ</param>
	void SetPlayerModel(Entity *entity);

	/// <summary>
	/// プレイヤーのパーティクルエミッタのセッター
	/// </summary>
	/// <param name="pEmitter">プレイヤーのパーティクルエミッタ</param>
	void SetParticleEmitter(ParticleEmitter *pEmitter) { pEmitter_ = pEmitter; }

	/// <summary>
	/// 再生トリガー状態ゲッター
	/// </summary>
	/// <returns>再生トリガー状態</returns>
	bool GetIsPlay() { return isPlay_; }

	/// <summary>
	/// 終了トリガーセッター
	/// </summary>
	/// <returns>終了トリガー</returns>
	bool GetIsEnd() { return isEnd_; }

public: // その他関数群

	/// <summary>
	/// ゴールアニメーション再生関数
	/// </summary>
	void PlayGoalAnim();

private: // プライベートなメンバ関数

	/// <summary>
	/// ベクトルを同時座標系に変換する関数
	/// </summary>
	/// <param name="v">ベクトル</param>
	/// <param name="m">行列</param>
	/// <returns>回転後ベクトル</returns>
	const Vector3 MatrixToVector(const Vector3 &v, const Matrix4x4 &m);

	// <summary>
	/// 調整項目クラスにメンバ変数のパラメータを追加する関数
	/// </summary>
	void AddItem(int num = 999);

	/// <summary>
	/// 調整項目クラスにメンバ変数のパラメータをセットする関数
	/// </summary>
	void SetItem(int num = 999);

	/// <summary>
	/// 調整項目クラスからメンバ変数のパラメータの値を読み込む関数
	/// </summary>
	void ApplyItem(int num = 999);

private: // メンバ変数

	// 調整項目クラス
	GlobalVariables *gv_ = nullptr;
	// カメラマネージャー
	CameraManager *cameraManager_ = nullptr;
	// ステージ選択マネージャー
	StageSelectManager *stageSelectManager_ = nullptr;

	// ゴール時のカメラ
	Camera3D *goalCamera_ = nullptr;
	// ゴール時のカメラのオフセット
	Vector3 offsetTranslate_ = { 0.0f, 1.5f, -5.0f }; // 位置
	Vector3 offsetRotate_ = { -0.1f, 0.0f, 0.2f };// 回転

	// プレイヤーモデル
	Entity *playerModel_ = nullptr;
	// プレイヤーを強制的にこの角度に向かせる
	float playerRotateY_ = 0.0f;

	// パーティクルの発生位置がズレてしまうので強制的に停止させる
	ParticleEmitter *pEmitter_;

	//　再生
	bool isPlay_ = false;

	// 終了トリガー
	bool isEnd_ = false;

	// クリア音再生トリガー
	bool clearSETrigger_ = false;
	// 効果音系
	static uint32_t clearEnterSE_; // ゴールに触れたときの音
	static uint32_t clearSE_; // クリア音

	Material backMaterial_;

#ifdef _DEBUG

	// 情報を追加するステージ番号
	int imGuiAddToStageNumber_ = 0;
	// 情報を読み込むステージ番号
	int ImGuiReadToStageNumber_ = 0;

#endif // _DEBUG

};

