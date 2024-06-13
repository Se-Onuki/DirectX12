#pragma once
#include "SceneManager.h"

#include "../Engine/DirectBase/3D/DirectionLight.h"
#include "../Engine/DirectBase/2D/Sprite.h"
#include "../Engine/DirectBase/Render/CameraAnimations/CameraManager.h"

#include "../Engine/DirectBase/Model/Model.h"
#include "../Utils/Math/Transform.h"

#include "../Engine/DirectBase/Input/Input.h"
#include "../Engine/DirectBase/Base/Audio.h"
#include "../ECS/World/World.hpp"
#include "../ECS/Entity/EntityManager.hpp"
#include "../ECS/MultiArray.h"

#include "../Utils/Graphics/Color.h"
#include "../Utils/IO/CSV.h"
#include "../../Header/Object/Particle.h"
#include "../Utils/Containers/ConstVector.h"
#include "../ECS/System/SystemManager.h"
#include "../Header/Object/Block/BlockManager.h"
#include "../Engine/DirectBase/Model/BoneModel.h"
#include "../Header/Object/Ground.h"
#include "../Header/Entity/Entity.h"
#include "../Header/Object/Particle/ParticleManager.h"
#include "../Utils/Containers/UnChainVector.h"
#include "../Header/Object/HealthBar.h"
#include "../ECS/Entity/Spawner.h"
#include "../Engine/DirectBase/PostEffect/OffScreenRendering.h"
#include "../Engine/ResourceObject/ResourceObjectManager.h"
#include "../ECS/Component/ComponentRegistry.h"
#include "../Engine/DirectBase/Texture/FullScreenTextureStrage.h"


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

	SolEngine::ResourceObjectManager<Shader, ShaderSource> *pShaderManager_;

	int32_t isGrayScale_ = 0;

	Input *input_ = nullptr;
	Audio *audio_ = nullptr;
	CameraManager *cameraManager_ = nullptr;

	DirectXCommon *pDxCommon_ = nullptr;

	std::unique_ptr<DirectionLight> light_ = nullptr;

	StructuredBuffer<Particle::ParticleData> particleArray_{ 1024u };

	BlockManager *blockRender_;
	SkinModelListManager *skinModelRender_;

	ECS::SystemManager systemManager_;

	Model *model_ = nullptr;
	Model *boxModel_ = nullptr;
	Model *playerModel_ = nullptr;

	std::unique_ptr<World> world_ = nullptr;
	ECS::EntityManager *entityManager_;
	ECS::Spawner spawner_;

	ECS::ComponentRegistry *compRegistry_;

	std::unique_ptr<ECS::Prefab> prefab_;

	std::unique_ptr<ECS::Prefab> playerPrefab_;

	Ground ground_{ 0.f, Vector2{100.f,100.f} };

	std::unique_ptr<ECS::Prefab> enemyPrefab_ = nullptr;

	SoLib::UnChainVector<uint32_t> unVector_;

	Camera3D *followCamera_;

	Audio::SoundHandle soundA_;

	BoneModel boneModel_;

	SoLib::DeltaTimer spawnTimer_{ 2.5f };

	std::array<BoneModel::SimpleTransform, 5u> boneTransform_;

	GameObject gameObject_;

	ParticleManager *particleManager_;

	std::unique_ptr<HealthBar> healthBar_;

	std::array<std::unique_ptr<HealthBar>, 50u> enemyHealthBar_;

	std::unique_ptr<SkinModel> skinModel_;

	Model *attackModel_ = nullptr;

	SoLib::DeltaTimer playerSpawn_{ 1.f, false };

	std::unique_ptr<Model> assimpModel_ = nullptr;

	ModelAnimation::Animation animation_;
	ModelAnimation::Animation attackAnimation_;

	std::unique_ptr<PostEffect::OffScreenRenderer> offScreen_ = nullptr;

	SolEngine::FullScreenTextureStrage *texStrage_;
	SolEngine::FullScreenTextureStrage::UniqueHandle offScreenTex_;
	PostEffect::FullScreenRenderer *fullScreen_ = nullptr;

};