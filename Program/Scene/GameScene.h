/// @file GameScene.h
/// @brief ゲームシーンの実装
/// @author ONUKI seiya

#pragma once
#include "SceneManager.h"

#include "../Engine/DirectBase/2D/Sprite.h"
#include "../Engine/DirectBase/3D/DirectionLight.h"
#include "../Engine/DirectBase/Render/CameraAnimations/CameraManager.h"

#include "../../Engine/Utils/Math/Transform.h"
#include "../Engine/DirectBase/Model/Model.h"

#include "../Engine/DirectBase/Base/Audio.h"
#include "../Engine/DirectBase/Input/Input.h"

#include "../../Engine/Utils/Containers/ConstVector.h"
#include "../../Engine/Utils/Containers/UnChainVector.h"
#include "../../Engine/Utils/Graphics/Color.h"
#include "../../Engine/Utils/IO/CSV.h"
#include "../../Header/Object/Particle.h"
#include "../Engine/DirectBase/DxResource/DxResourceBufferPool.h"
#include "../Engine/DirectBase/Model/BoneModel.h"
#include "../Engine/DirectBase/PostEffect/OffScreenRendering.h"
#include "../Engine/DirectBase/Render/ModelInstancingRender/ModelInstancingRender.h"
#include "../Engine/DirectBase/Texture/FullScreenTextureStrage.h"
#include "../Engine/ECS/Component/ComponentRegistry.h"
#include "../Engine/ECS/Entity/Spawner.h"
#include "../Engine/ECS/System/FunctionalSystem.h"
#include "../Engine/ECS/World/ComponentArray/Chunk.h"
#include "../Engine/ResourceObject/ResourceObjectManager.h"
#include "../Header/Entity/Entity.h"
#include "../Header/Object/Block/BlockManager.h"
#include "../Header/Object/Ground.h"
#include "../Header/Object/HealthBar.h"
#include "../Header/Object/PlayerLevel/LevelUP.h"
#include "../Header/Object/Fade.h"
#include "../Game/Resource/GameScore.h"
#include "../Engine/UI/Text/NumberRender.h"
#include "../Engine/DirectBase/File/GlobalVariables.h"
#include "../Game/Resource/EnemyDataTable.h"

/// @class GameScene
/// @brief ゲームシーン
class GameScene : public SolEngine::IScene {
public:
	/// @fn GameScene(void)
	/// @brief コンストラクタ
	/// @details 各種リソースオブジェクトを初期化する
	GameScene();
	/// @fn ~GameScene(void)
	/// @brief デストラクタ
	/// @details 各種リソースオブジェクトの解放をする
	~GameScene();

	/// @fn void OnEnter(void)
	/// @brief シーンの初期化処理
	/// @details シーンの入室時に呼び出される
	void OnEnter() override;
	/// @fn void OnExit(void)
	/// @brief シーンの終了処理
	/// @details シーンの退室時に呼び出される
	void OnExit() override;

	/// @fn void Update(void)
	/// @brief シーンの更新処理
	/// @details シーンの更新時に呼び出される
	void Update() override;

	/// @fn void Draw(void)
	/// @brief シーンの描画処理
	/// @details シーンの描画時に呼び出される
	void Draw() override;

	/// @fn void PostEffectSetup(void)
	/// @brief ポストエフェクトの設定処理
	/// @details シーンの描画処理の前に呼び出される
	void PostEffectSetup() override;

	/// @fn void PostEffectEnd(void)
	/// @brief ポストエフェクトの終了処理
	/// @details シーンの描画処理の後に呼び出される
	void PostEffectEnd() override;

private:

	/// @brief ゲームスコアを保存する
	void SetGameScore();

	/// @brief ゲームのリザルトを取得する
	const GameScore &GetGameScore() const;

	/// @brief 初期化処理
	void FlameClear();

	/// @brief プレイヤの死亡判定
	/// @param world[in] ワールドデータ 
	/// @param playerTimer[in, out] タイマー 
	/// @param scene[out] シーンマネージャー 
	/// @param fade[out] フェード処理 
	void PlayerDead(const ECS::World &world, SoLib::DeltaTimer &playerTimer, SolEngine::SceneManager *const scene, Fade *const fade) const;

	/// @brief プレイヤの範囲攻撃の生成処理
	/// @param world[in, out] ワールドデータ
	void GeneratePlayerRangeAttack(ECS::World &world) const;

	/// @brief プレイヤの範囲攻撃の生成処理
	/// @param world[in, out] ワールドデータ
	void GeneratePlayerStoneAttack(ECS::World &world, uint32_t addCount) const;

	/// @brief プレイヤの範囲攻撃の生成処理
	/// @param world[in, out] ワールドデータ
	void GeneratePlayerArrowAttack(ECS::World &world) const;

	/// @brief 経験値の生成
	/// @param world[in, out] ワールドデータ
	/// @param killCount[in, out] 倒した数
	void GenerateExperience(ECS::World &world, size_t &killCount) const;

	/// @brief 経験値の加算
	/// @param world[in, out] ワールドデータ
	void PlayerExperience(ECS::World &world) const;

	/// @brief パーティクルの描画
	/// @param world[in] ワールドデータ
	/// @param attackRender[out] 描画バッファ
	void PatricleRender(const ECS::World &world, SolEngine::ModelInstancingRender &attackRender) const;

	/// @brief 衛星の描画
	/// @param world[in] ワールドデータ
	/// @param attackRender[out] 描画バッファ
	void SatelliteAttackRender(const ECS::World &world, SolEngine::ModelInstancingRender &attackRender) const;

	/// @brief 攻撃判定の描画
	/// @param world[in] ワールドデータ
	/// @param attackRender[out] 描画バッファ
	void AttackEffectRender(const ECS::World &world, SolEngine::ModelInstancingRender &attackRender) const;

	/// @brief 攻撃判定の描画
	/// @param world[in] ワールドデータ
	/// @param attackRender[out] 描画バッファ
	void ArrowAttackEffectRender(const ECS::World &world, SolEngine::ModelInstancingRender &attackRender) const;

	/// @brief タイマーに応じてスポナーに追加する処理
	/// @param timer[in, out] タイマー 
	/// @param spawner[out] スポナー 
	void AddSpawner(SoLib::DeltaTimer &timer, ECS::Spawner &spawner) const;

	/// @brief ダメージの描画
	/// @param world[in] ワールドデータ
	/// @param numberRender[out] 数字の描画データ格納先
	void DamageRender(const ECS::World &world, const SolEngine::Camera3D &camera, SolEngine::NumberRender &numberRender) const;

	bool ImGuiWidget(const std::string_view &name);

	void Load(const GlobalVariables::Group &group);

	void Save(GlobalVariables::Group &group) const;

private:
	/// @brief メニューのタイマー
	SoLib::DeltaTimer menuTimer_;

	// レベルアップのUIのサイズ
	VItem(Vector2, LevelUpUISize, _) {};
	// 経験値バーのUIの中心位置の画面サイズに対する比率
	VItem(Vector2, ExpUICentorMul, _) { { 0.5f, 1.f } };
	// 経験値バーのUIの中心位置のピクセル単位の位置
	VItem(Vector2, ExpUICentorDiff, _) { { 0.f, -16.f } };
	// 経験値バーのUIのサイズの画面サイズに対する比率
	VItem(Vector2, ExpUIScaleMul, _) { { 1.f, 0.f } };
	// 経験値バーのUIのサイズのピクセル単位のサイズ
	VItem(Vector2, ExpUIScaleDiff, _) { { 0.f, 32.f } };

	VItem(int32_t, EnemyHealthBase, _) { 100 };
	VItem(int32_t, EnemyHealthDiff, _) { 500 };

	VItem(int32_t, EnemySpawnCount, _) { 1 };
	VItem(int32_t, EnemySpawnDiff, _) { 20 };
	VItem(float, EnemyRadius, _) { 45.f };

	VItem(float, PowerUpEnemySpawnTime, _) { 60.f };

	// 色
	inline static constexpr std::array<uint32_t, 6> kEnemyColor_{
		0xFFFFFFFF,
		0x55FF55FF,
		0x5555FFFF,
		0xFF33FFFF,
		0xFFFF00FF,
	};

	// 敵の強さのテーブル
	std::unique_ptr<EnemyTable> enemyTable_;

	// ゲームのスコアを返す
	GameScore gameScore_;

	/// @brief メニューの表示フラグ
	bool isMenuOpen_ = false;
	/// @brief シェーダーのリソースマネージャ
	SolEngine::ResourceObjectManager<Shader> *pShaderManager_ = nullptr;
	/// @brief グレースケールが有効か
	int32_t isGrayScale_ = 0;
	/// @brief 入力のインスタンス
	SolEngine::Input *input_ = nullptr;
	/// @brief オーディオのインスタンス
	SolEngine::Audio *audio_ = nullptr;
	/// @brief カメラマネージャーのインスタンス
	CameraManager *cameraManager_ = nullptr;
	/// @brief DirectXの管理クラスのインスタンス
	DirectXCommon *pDxCommon_ = nullptr;
	/// @brief ライトのデータ
	std::unique_ptr<DirectionLight> light_ = nullptr;
	/// @brief パーティクルの配列
	StructuredBuffer<Particle::ParticleData> particleArray_{ 1024u };
	/// @brief ブロックマネージャーのインスタンス
	BlockManager *blockRender_ = nullptr;
	/// @brief スキンモデルマネージャーのインスタンス
	SkinModelListManager *skinModelRender_ = nullptr;
	/// @brief モデルハンドルマネージャーのインスタンス
	ModelHandleListManager *modelHandleRender_ = nullptr;
	/// @brief スキンモデルハンドルマネージャーのインスタンス
	SkinModelHandleListManager *skinModelHandleRender_ = nullptr;
	/// @brief ゴーストモデルのインスタンシング描画の配列
	SolEngine::ModelInstancingRender ghostRenderer_;
	/// @brief 影モデルのインスタンシング描画の配列
	SolEngine::ModelInstancingRender shadowRenderer_;

	/// @brief 経験値のインスタンシング描画の配列
	SolEngine::ModelInstancingRender expRender_;
	/// @brief 攻撃モデルのインスタンシング描画の配列
	SolEngine::ModelInstancingRender attackRender_;

	/// @brief 矢のモデルのインスタンシング描画の配列
	SolEngine::ModelInstancingRender arrowAttackRender_;

	/// @brief 箱のモデルのインスタンシング描画の配列
	SolEngine::ModelInstancingRender boxAttackRender_;

	/// @brief スポナークラス
	ECS::Spawner spawner_;
	/// @brief ECSのワールド
	ECS::World newWorld_;
	/// @brief システムの実行クラス
	ECS::SystemExecuter systemExecuter_;
	/// @brief コンポーネントレジストリ
	ECS::ComponentRegistry *compRegistry_ = nullptr;
	/// @brief レベルアップ時に表示されるUI
	std::unique_ptr<LevelUP> levelUpUI_;
	/// @brief ECSのプレハブ
	std::unique_ptr<ECS::Prefab> prefab_;
	/// @brief プレイヤーのプレハブ
	std::unique_ptr<ECS::Prefab> playerPrefab_;
	/// @brief 地面のデータ
	Ground ground_{ 0.f, Vector2{100.f, 100.f} };
	/// @brief 敵のプレハブ
	std::unique_ptr<ECS::Prefab> enemyPrefab_ = nullptr;
	/// @brief カメラのアドレス
	SolEngine::Camera3D *followCamera_ = nullptr;
	/// @brief 音のリソースハンドル
	SolEngine::Audio::SoundHandle attackSound_;
	/// @brief スポーン間隔を管理するタイマー
	SoLib::DeltaTimer spawnTimer_{ 2.5f };
	/// @brief ゲームオブジェクト
	GameObject gameObject_;

	/// @brief 体力バー
	std::unique_ptr<HealthBar> healthBar_;
	/// @brief 経験値バー
	std::unique_ptr<HealthBar> expBar_;
	/// @brief レベルUI
	std::unique_ptr<Sprite> levelUI_;
	/// @brief 敵の体力バーの配列
	std::array<std::unique_ptr<HealthBar>, 50u> enemyHealthBar_;
	/// @brief スキニングモデル
	std::unique_ptr<SkinModel> skinModel_;
	/// @brief スキンモデルのリスト
	std::list<std::unique_ptr<SkinModel>> skinModelList_;
	/// @brief 攻撃モデルのアドレス
	Model *attackModel_ = nullptr;
	/// @brief プレイヤスポナーのタイマー
	SoLib::DeltaTimer playerSpawn_{ 1.f, false };
	/// @brief アニメーションのハンドル
	SolEngine::ResourceHandle<SolEngine::Animation> animation_;
	/// @brief 攻撃アニメーションのハンドル
	SolEngine::ResourceHandle<SolEngine::Animation> attackAnimation_;
	/// @brief オフスクリーンレンダラの実体
	std::unique_ptr<PostEffect::OffScreenRenderer> offScreen_ = nullptr;
	/// @brief テクスチャストレージのインスタンス
	SolEngine::FullScreenTextureStrage *texStrage_ = nullptr;
	/// @brief フルスクリーンレンダラのインスタンス
	PostEffect::FullScreenRenderer *fullScreen_ = nullptr;
	/// @brief ダメージのタイマー
	SoLib::Time::DeltaTimer damageTimer_{ 0.5f };
	/// @brief ヴィネッティングのパラメータ
	CBuffer<std::pair<float, float>> vignettingParam_{};
	/// @brief グレースケールのパラメータ
	CBuffer<float> grayScaleParam_;
	/// @brief hsvの調整パラメータ
	CBuffer<SoLib::Color::HSV4> hsvParam_{ {0.f, 0.5f, 0.5f, 1.f} };
	/// @brief ガウシアンブラーのパラメータ
	CBuffer<std::pair<float, int32_t>> gaussianParam_;

	// 数字の出力
	std::unique_ptr<SolEngine::NumberRender> numberRender_;

	// 影の色
	SoLib::Color::RGB4 shadowColor_ = 0x00000055;
	// 経験値の色
	SoLib::Color::RGB4 expColor_ = 0x555500FF;

	// 攻撃時間
	float attackTime_ = 0.25f;
	// ノックバック威力
	float baseKnockBackPower_ = 0.15f;

	// 飛び道具の回転の速度
	float arrowAttackRotSpeed_ = 0.5f;

	float captureRange_ = 3.f;

	float captureRangeLevelUp_ = 0.5f;

	std::vector<std::unique_ptr<ButtonUI>> arrowLevelUp_;

	// 時間表示のUI
	std::array<std::unique_ptr<SolEngine::NumberText>, 2u> gameTimerUI_;
	// 時間の中間の点
	std::unique_ptr<Sprite> timerCoron_;

	SoLib::DeltaTimer gameTimer_{ 60 * 3 };

	std::unique_ptr<SolEngine::NumberText> killUI_;

	size_t killCount_;
};