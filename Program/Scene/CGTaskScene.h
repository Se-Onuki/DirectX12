/// @file CGTaskScene.h
/// @brief CGの課題のシーン
/// @author ONUKI seiya
#pragma once
#include "SceneManager.h"
#include <memory>
#include "../Engine/DirectBase/3D/DirectionLight.h"
#include "../Header/Entity/Entity.h"
#include "../Engine/DirectBase/PostEffect/OffScreenRendering.h"
#include "../Engine/DirectBase/Model/Model.h"
#include "../../Engine/Utils/Containers/AlignasWrapper.h"
#include "../Engine/DirectBase/Model/ModelData.h"
#include "../Engine/DirectBase/ComputeShader/SkinningComputeShader.h"
#include "../Engine/DirectBase/Render/SkyBox/SkyBox.h"
#include "../Engine/DirectBase/Model/SkeletonAnimation/Skeleton.h"
#include "../Engine/DirectBase/Model/SkeletonAnimation/ModelAnimation.h"
#include "../Engine/DirectBase/SkinAnimation/SkinAnimation.h"

class CGTaskScene : public SolEngine::IScene {
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

	std::unique_ptr<SolEngine::SkyBox> skyBox_;

	SoLib::Transform skyBoxTransform_;
	SoLib::Transform transform_;

	SolEngine::ResourceHandle<SolEngine::ModelData> boxModel_;

	DirectXCommon *pDxCommon_ = nullptr;

};