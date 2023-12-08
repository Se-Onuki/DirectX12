#pragma once
#include "SceneManager.h"

#include "../Engine/DirectBase/3D/DirectionLight.h"
#include "../Engine/DirectBase/2D/Sprite.h"

#include "../Engine/DirectBase/Model/ModelManager.h"
#include "../Engine/DirectBase/Base/Audio.h"

/// <summary>
/// モデルのロード等を行うシーン
/// </summary>
class LoadScene : public IScene
{
public: // コンストラクタ等
	// コンストラクタ
	LoadScene();
	// デストラクタ
	~LoadScene();

public: // メンバ関数

	/// <summary>
	/// シーンに入った際に呼び出される関数
	/// </summary>
	void OnEnter() override;
	/// <summary>
	/// シーンから退出する際に呼び出される乱数
	/// </summary>
	void OnExit() override;

	/// <summary>
	/// 更新関数
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画関数
	/// </summary>
	void Draw() override;

private: // メンバ変数

	// 音インスタンス
	Audio* audio_ = nullptr;
	// モデルマネージャインスタンス
	ModelManager* modelManager_ = nullptr;

};

