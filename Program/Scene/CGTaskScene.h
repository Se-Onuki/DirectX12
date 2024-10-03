#pragma once
#include "SceneManager.h"
#include <memory>
#include "../Engine/DirectBase/3D/DirectionLight.h"
#include "../Header/Entity/Entity.h"
#include "../Engine/DirectBase/PostEffect/OffScreenRendering.h"
#include "../Engine/DirectBase/Model/Model.h"
#include "../../Engine/Utils/Containers/AlignasWrapper.h"
#include "../Engine/ECS/World/World.hpp"
#include "../Engine/ECS/System/SystemManager.h"
#include "../Engine/DirectBase/Model/ModelData.h"
#include "../Engine/DirectBase/ComputeShader/SkinningComputeShader.h"
#include "../Engine/DirectBase/Render/SkyBox/SkyBox.h"
#include "../Engine/DirectBase/Model/SkeletonAnimation/Skeleton.h"

class CGTaskScene : public IScene {
public:
	void OnEnter() override;
	void OnExit() override;

	void Update() override;
	void Draw() override;

	/// @brief ポストエフェクトの設定処理
	void PostEffectSetup() override;

	/// @brief ポストエフェクトの終了処理
	void PostEffectEnd() override;

private:
	std::unique_ptr<DirectionLight> light_ = nullptr;

	std::unique_ptr<World> world_ = nullptr;
	ECS::EntityManager *entityManager_;

	ECS::SystemManager systemManager_;

	std::unique_ptr<SkinModel> skinModel_;

	std::unique_ptr<SolEngine::SkyBox> skyBox_;

	Transform skyBoxTransform_;
	Transform transform_;

	std::unique_ptr<PostEffect::OffScreenRenderer> offScreen_ = nullptr;
	PostEffect::FullScreenRenderer *fullScreen_ = nullptr;

	SolEngine::AnimationPlayer animationPlayer_;
	SolEngine::ResourceHandle<SolEngine::Animation> animation_;

	SolEngine::ResourceHandle<SolEngine::ModelData> boxModel_;
	SolEngine::ResourceHandle<SolEngine::Skeleton> skeleton_;

	DirectXCommon *pDxCommon_ = nullptr;

	SolEngine::SkinningComputeShader computeShader_;

};