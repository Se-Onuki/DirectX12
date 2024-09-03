#include "SkinAnimation.h"

#include <algorithm>
#include <execution>

namespace SolEngine {

	void AnimationPlayer::Start(bool isLoop)
	{
		// アニメーションの初期化
		animationTimer_.Start(animation_->duration_, isLoop); // 時間を設定する
	}

	void AnimationPlayer::Update(float deltaTime)
	{
		// Animationの更新
		animationTimer_.Update(deltaTime);

		/*	CalcTransform(animationTimer_.GetNowFlame(), model);
			model->UpdateMatrix();*/
	}

	void AnimationPlayer::CalcTransform(float animateTime, ModelNode *const modelNode) const
	{
		// 対応したアニメーションがあれば通す
		if (animation_->nodeAnimations_.contains(modelNode->name_)) {
			// モデルのノードに紐づいたアニメーションを取得する
			const auto &rootNodeAnimation = animation_->nodeAnimations_.at(modelNode->name_);
			// ノードのアニメーションのデータを取得する
			SimpleTransformQuaternion rootTransform;
			if (rootNodeAnimation.scale_.size()) {
				rootTransform.scale_ = rootNodeAnimation.scale_.CalcValue(animateTime);
			}
			if (rootNodeAnimation.rotate_.size()) {
				rootTransform.rotate_ = rootNodeAnimation.rotate_.CalcValue(animateTime);
			}
			if (rootNodeAnimation.translate_.size()) {
				rootTransform.translate_ = rootNodeAnimation.translate_.CalcValue(animateTime);
			}
			// モデルデータの転送
			(*modelNode->localMatrix_) = rootTransform.Affine();
		}

		std::for_each(std::execution::par_unseq, modelNode->children_.begin(), modelNode->children_.end(), [this, animateTime](std::unique_ptr<ModelNode> &child) { CalcTransform(animateTime, child.get()); });

	}

}
