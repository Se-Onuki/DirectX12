#include "SkinAnimation.h"

#include <algorithm>
#include <execution>

namespace SolEngine {
	std::unique_ptr<Animation> Animation::Create(const SolEngine::AssimpData *assimpData, uint32_t index)
	{
		std::unique_ptr<Animation> result = std::make_unique<Animation>();

		const aiScene *scene = assimpData->importer_->GetScene();

		assert(scene->mNumAnimations != 0 and "アニメーションがありません｡");

		std::span<aiAnimation *>animationAssimp{ scene->mAnimations, scene->mNumAnimations }; // 一旦最初のアニメーションだけ採用する｡ そのうち複数対応するように｡

		// 時間の単位を秒単位に変更
		result->duration_ = static_cast<float>(animationAssimp[index]->mDuration / animationAssimp[index]->mTicksPerSecond);
		// mTicksPerSecond	: 周波数｡ 単位はHz｡
		// mDuration		: mTicksPerSecondで指定された周波数における長さ

		// assimpでは個々のAnimationをchannelと呼んでいるので､channelを回してNodeAnimationの情報を取ってくる｡
		{
			for (uint32_t channelIndex = 0u; channelIndex < animationAssimp[index]->mNumChannels; channelIndex++) {
				// アニメーションのデータのポインタ
				aiNodeAnim *nodeAnimationAssimp = animationAssimp[index]->mChannels[channelIndex];
				// アニメーションの名前から､紐づいた保存先を作成
				NodeAnimation &nodeAnimation = result->nodeAnimations_[nodeAnimationAssimp->mNodeName.C_Str()];

				// 座標データを取得していく
				for (uint32_t keyIndex = 0u; keyIndex < nodeAnimationAssimp->mNumPositionKeys; keyIndex++) {
					// assimp側の座標データ
					aiVectorKey &keyAssimp = nodeAnimationAssimp->mPositionKeys[keyIndex];

					// キーフレームの保存先
					KeyFlameVector3 keyFlame;
					keyFlame.time_ = static_cast<float>(keyAssimp.mTime / animationAssimp[index]->mTicksPerSecond); // 周波数から秒単位に変換
					keyFlame.kValue_ = Vector3{ -keyAssimp.mValue.x, keyAssimp.mValue.y, keyAssimp.mValue.z };  // 右手から左手に変更

					// データを転送
					nodeAnimation.translate_.keyFlames_.push_back(std::move(keyFlame));
				}

				// 姿勢データを取得していく
				for (uint32_t keyIndex = 0u; keyIndex < nodeAnimationAssimp->mNumRotationKeys; keyIndex++) {
					// assimp側の姿勢データ
					aiQuatKey &keyAssimp = nodeAnimationAssimp->mRotationKeys[keyIndex];

					// キーフレームの保存先
					KeyFlameQuaternion keyFlame;
					keyFlame.time_ = static_cast<float>(keyAssimp.mTime / animationAssimp[index]->mTicksPerSecond);                        // 周波数から秒単位に変換
					keyFlame.kValue_ = Quaternion{ keyAssimp.mValue.x, -keyAssimp.mValue.y, -keyAssimp.mValue.z, keyAssimp.mValue.w }; // 右手から左手に変更

					// データを転送
					nodeAnimation.rotate_.keyFlames_.push_back(std::move(keyFlame));
				}

				// スケールのデータを取得していく
				for (uint32_t keyIndex = 0u; keyIndex < nodeAnimationAssimp->mNumScalingKeys; keyIndex++) {
					// assimp側の姿勢データ
					aiVectorKey &keyAssimp = nodeAnimationAssimp->mScalingKeys[keyIndex];

					// キーフレームの保存先
					KeyFlameVector3 keyFlame;
					keyFlame.time_ = static_cast<float>(keyAssimp.mTime / animationAssimp[index]->mTicksPerSecond); // 周波数から秒単位に変換
					keyFlame.kValue_ = Vector3{ keyAssimp.mValue.x, keyAssimp.mValue.y, keyAssimp.mValue.z };   // そのまま代入する

					// データを転送
					nodeAnimation.scale_.keyFlames_.push_back(std::move(keyFlame));
				}
			}
		}

		return std::move(result);
	}

	void AnimationPlayer::Start(bool isLoop)
	{
		// アニメーションの初期化
		animationTimer_.Start(animation_->duration_, isLoop); // 時間を設定する
	}

	void AnimationPlayer::Update(float deltaTime, SolEngine::ModelData *model)
	{
		// Animationの更新
		animationTimer_.Update(deltaTime);

		CalcTransform(animationTimer_.GetNowFlame(), model->rootNode_.get());
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
