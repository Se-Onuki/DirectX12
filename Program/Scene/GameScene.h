#pragma once
#include "SceneManager.h"

#include "../Engine/DirectBase/3D/DirectionLight.h"
#include "../Engine/DirectBase/2D/Sprite.h"
#include "../Engine/DirectBase/Render/CameraAnimations/CameraManager.h"

#include "../Engine/DirectBase/Model/Model.h"
#include "../../Engine/Utils/Math/Transform.h"

#include "../Engine/DirectBase/Input/Input.h"
#include "../Engine/DirectBase/Base/Audio.h"
#include "../Engine/ECS/World/World.hpp"
#include "../Engine/ECS/Entity/EntityManager.hpp"
#include "../Engine/ECS/MultiArray.h"

#include "../../Engine/Utils/Graphics/Color.h"
#include "../../Engine/Utils/IO/CSV.h"
#include "../../Header/Object/Particle.h"
#include "../../Engine/Utils/Containers/ConstVector.h"
#include "../Engine/ECS/System/SystemManager.h"
#include "../Header/Object/Block/BlockManager.h"
#include "../Engine/DirectBase/Model/BoneModel.h"
#include "../Header/Object/Ground.h"
#include "../Header/Entity/Entity.h"
#include "../Header/Object/Particle/ParticleManager.h"
#include "../../Engine/Utils/Containers/UnChainVector.h"
#include "../Header/Object/HealthBar.h"
#include "../Engine/ECS/Entity/Spawner.h"
#include "../Engine/DirectBase/PostEffect/OffScreenRendering.h"
#include "../Engine/ResourceObject/ResourceObjectManager.h"
#include "../Engine/ECS/Component/ComponentRegistry.h"
#include "../Engine/DirectBase/Texture/FullScreenTextureStrage.h"
#include "../Engine/DirectBase/DxResource/DxResourceBufferPool.h"
#include "../Engine/ECS/World/ComponentArray/Chunk.h"
#include "../Engine/ECS/System/FunctionalSystem.h"
#include "../Header/Object/PlayerLevel/LevelUP.h"
#include "../Engine/DirectBase/Render/ModelInstancingRender/ModelInstancingRender.h"

class GameScene : public IScene {
public:
	GameScene();
	~GameScene();

	void OnEnter() override;
	void OnExit() override;

	void Update() override;
	void Draw() override;

	/// @brief ポストエフェクトの設定処理
	void PostEffectSetup() override;

	/// @brief ポストエフェクトの終了処理
	void PostEffectEnd() override;


private:

	SoLib::DeltaTimer menuTimer_;

	bool isMenuOpen_;

	SolEngine::ResourceObjectManager<Shader> *pShaderManager_;

	int32_t isGrayScale_ = 0;

	Input *input_ = nullptr;
	Audio *audio_ = nullptr;
	CameraManager *cameraManager_ = nullptr;

	DirectXCommon *pDxCommon_ = nullptr;

	std::unique_ptr<DirectionLight> light_ = nullptr;

	StructuredBuffer<Particle::ParticleData> particleArray_{ 1024u };

	BlockManager *blockRender_;
	SkinModelListManager *skinModelRender_;
	ModelHandleListManager *modelHandleRender_;
	SkinModelHandleListManager *skinModelHandleRender_;

	SolEngine::ModelInstancingRender ghostRenderer_;
	SolEngine::ModelInstancingRender shadowRenderer_;

	//ECS::SystemManager systemManager_;

	Model *model_ = nullptr;
	Model *boxModel_ = nullptr;
	Model *playerModel_ = nullptr;

	// スポナークラス
	ECS::Spawner spawner_;
	// ECSのワールド
	ECS::World newWorld_;
	// システムの実行クラス
	ECS::SystemExecuter systemExecuter_;
	// コンポーネントレジストリ
	ECS::ComponentRegistry *compRegistry_;

	std::unique_ptr<LevelUP> levelUpUI_;

	std::unique_ptr<ECS::Prefab> prefab_;

	std::unique_ptr<ECS::Prefab> playerPrefab_;

	Ground ground_{ 0.f, Vector2{100.f,100.f} };

	std::unique_ptr<ECS::Prefab> enemyPrefab_ = nullptr;

	SoLib::UnChainVector<uint32_t> unVector_;

	Camera3D *followCamera_;

	Audio::SoundHandle soundA_;

	SoLib::DeltaTimer spawnTimer_{ 5.f };

	std::array<BoneModel::SimpleTransform, 5u> boneTransform_;

	GameObject gameObject_;

	ParticleManager *particleManager_;

	std::unique_ptr<HealthBar> healthBar_;
	std::unique_ptr<HealthBar> expBar_;
	std::unique_ptr<Sprite> levelUI_;

	std::array<std::unique_ptr<HealthBar>, 50u> enemyHealthBar_;

	std::unique_ptr<SkinModel> skinModel_;

	std::list<std::unique_ptr<SkinModel>> skinModelList_;

	Model *attackModel_ = nullptr;

	SoLib::DeltaTimer playerSpawn_{ 1.f, false };

	std::unique_ptr<Model> assimpModel_ = nullptr;

	SolEngine::ResourceHandle<SolEngine::Animation> animation_;
	SolEngine::ResourceHandle<SolEngine::Animation> attackAnimation_;

	std::unique_ptr<PostEffect::OffScreenRenderer> offScreen_ = nullptr;

	SolEngine::FullScreenTextureStrage *texStrage_;
	PostEffect::FullScreenRenderer *fullScreen_ = nullptr;

	SoLib::Time::DeltaTimer damageTimer_{ 0.5f };

	CBuffer<std::pair<float, float>> vignettingParam_{};
	CBuffer<float> grayScaleParam_;
	CBuffer<SoLib::Color::HSV4> hsvParam_{ { 0.f,0.5f,0.5f,1.f } };
	CBuffer<std::pair<float, int32_t>> gaussianParam_;

};