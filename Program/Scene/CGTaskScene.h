#pragma once
#include "SceneManager.h"
#include <memory>
#include "../Engine/DirectBase/3D/DirectionLight.h"
#include "../Header/Entity/Entity.h"
#include "../Engine/DirectBase/PostEffect/OffScreenRendering.h"
#include "../Engine/DirectBase/Model/Model.h"
#include "../Utils/Containers/AlignasWrapper.h"

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

	Model *model_;

	Model *uvModel_;

	std::unique_ptr<SkinModel> skinModel_;

	std::unique_ptr<AlignasWrapper<Vector2>> vec2_;

	Transform transform_;

	PostEffect::OffScreenRenderer *offScreen_ = nullptr;
	PostEffect::FullScreenRenderer *fullScreen_ = nullptr;

	ModelAnimation::AnimationPlayer animationPlayer_;
	ModelAnimation::Animation animation_;

	DirectXCommon *pDxCommon_ = nullptr;

};