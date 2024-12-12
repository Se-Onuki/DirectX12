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
#include "../Header/Object/Particle/ParticleManager.h"
#include "../Header/Object/PlayerLevel/LevelUP.h"

/// @class GameScene
/// @brief ゲームシーン
class GameScene : public IScene {
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
    /// @brief メニューのタイマー
    SoLib::DeltaTimer menuTimer_;

    /// @brief メニューの表示フラグ
    bool isMenuOpen_;
    /// @brief シェーダーのリソースマネージャ
    SolEngine::ResourceObjectManager<Shader> *pShaderManager_;
    /// @brief グレースケールが有効か
    int32_t isGrayScale_ = 0;
    /// @brief 入力のインスタンス
    Input *input_ = nullptr;
    /// @brief オーディオのインスタンス
    Audio *audio_ = nullptr;
    /// @brief カメラマネージャーのインスタンス
    CameraManager *cameraManager_ = nullptr;
    /// @brief DirectXの管理クラスのインスタンス
    DirectXCommon *pDxCommon_ = nullptr;
    /// @brief ライトのデータ
    std::unique_ptr<DirectionLight> light_ = nullptr;
    /// @brief パーティクルの配列
    StructuredBuffer<Particle::ParticleData> particleArray_{1024u};
    /// @brief ブロックマネージャーのインスタンス
    BlockManager *blockRender_;
    /// @brief スキンモデルマネージャーのインスタンス
    SkinModelListManager *skinModelRender_;
    /// @brief モデルハンドルマネージャーのインスタンス
    ModelHandleListManager *modelHandleRender_;
    /// @brief スキンモデルハンドルマネージャーのインスタンス
    SkinModelHandleListManager *skinModelHandleRender_;
    /// @brief ゴーストモデルのインスタンシング描画の配列
    SolEngine::ModelInstancingRender ghostRenderer_;
    /// @brief 影モデルのインスタンシング描画の配列
    SolEngine::ModelInstancingRender shadowRenderer_;

    /// @brief 経験値のインスタンシング描画の配列
    SolEngine::ModelInstancingRender expRender_;
    /// @brief 攻撃モデルのインスタンシング描画の配列
    SolEngine::ModelInstancingRender attackRender_;

    /// @brief スポナークラス
    ECS::Spawner spawner_;
    /// @brief ECSのワールド
    ECS::World newWorld_;
    /// @brief システムの実行クラス
    ECS::SystemExecuter systemExecuter_;
    /// @brief コンポーネントレジストリ
    ECS::ComponentRegistry *compRegistry_;
    /// @brief レベルアップ時に表示されるUI
    std::unique_ptr<LevelUP> levelUpUI_;
    /// @brief ECSのプレハブ
    std::unique_ptr<ECS::Prefab> prefab_;
    /// @brief プレイヤーのプレハブ
    std::unique_ptr<ECS::Prefab> playerPrefab_;
    /// @brief 地面のデータ
    Ground ground_{0.f, Vector2{100.f, 100.f}};
    /// @brief 敵のプレハブ
    std::unique_ptr<ECS::Prefab> enemyPrefab_ = nullptr;
    /// @brief カメラのアドレス
    Camera3D *followCamera_;
    /// @brief 音のリソースハンドル
    Audio::SoundHandle soundA_;
    /// @brief スポーン間隔を管理するタイマー
    SoLib::DeltaTimer spawnTimer_{2.5f};
    /// @brief ゲームオブジェクト
    GameObject gameObject_;
    /// @brief パーティクルマネージャーのインスタンス
    ParticleManager *particleManager_;

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
    SoLib::DeltaTimer playerSpawn_{1.f, false};
    /// @brief アニメーションのハンドル
    SolEngine::ResourceHandle<SolEngine::Animation> animation_;
    /// @brief 攻撃アニメーションのハンドル
    SolEngine::ResourceHandle<SolEngine::Animation> attackAnimation_;
    /// @brief オフスクリーンレンダラの実体
    std::unique_ptr<PostEffect::OffScreenRenderer> offScreen_ = nullptr;
    /// @brief テクスチャストレージのインスタンス
    SolEngine::FullScreenTextureStrage *texStrage_;
    /// @brief フルスクリーンレンダラのインスタンス
    PostEffect::FullScreenRenderer *fullScreen_ = nullptr;
    /// @brief ダメージのタイマー
    SoLib::Time::DeltaTimer damageTimer_{0.5f};
    /// @brief ヴィネッティングのパラメータ
    CBuffer<std::pair<float, float>> vignettingParam_{};
    /// @brief グレースケールのパラメータ
    CBuffer<float> grayScaleParam_;
    /// @brief hsvの調整パラメータ
    CBuffer<SoLib::Color::HSV4> hsvParam_{{0.f, 0.5f, 0.5f, 1.f}};
    /// @brief ガウシアンブラーのパラメータ
    CBuffer<std::pair<float, int32_t>> gaussianParam_;
};