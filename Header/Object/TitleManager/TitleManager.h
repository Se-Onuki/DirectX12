#pragma once
#include "../../Entity/Entity.h"

/// <summary>
/// タイトル画面のマネージャー
/// </summary>
class TitleManager
{
public: // メンバ関数

	/// <summary>
	/// 初期化関数
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新関数
	/// </summary>
	/// <param name="deltaTime">経過秒数</param>
	void Update(float deltaTime);

	/// <summary>
	/// 3Dモデル描画関数
	/// </summary>
	/// <param name="camera">カメラ</param>
	void Draw(const Camera3D& camera);

private: // メンバ変数

	// 天球
	std::unique_ptr<Entity> skyDome_;

	// プレイヤー
	std::unique_ptr<Entity> playerModel_;

};

