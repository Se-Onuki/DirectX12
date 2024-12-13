/// @file ModelAnimation.cpp
/// @brief モデルのアニメーションのリソース
/// @author ONUKI seiya
#include "ModelAnimation.h"

namespace SolEngine {

	std::unique_ptr<Animation> ResourceCreater<Animation>::CreateObject(const ResourceSource<Animation> &source) const {

		using namespace SkeletonAnimation;
		// データの作成
		std::unique_ptr<Animation> result = std::make_unique<Animation>();

		const aiScene *scene = source.assimpHandle_->importer_->GetScene();

		assert(scene->mNumAnimations != 0 and "アニメーションがありません｡");

		// アニメーションの配列
		std::span<aiAnimation *> animationAssimp = std::span<aiAnimation *>{ scene->mAnimations, scene->mNumAnimations };

		// 検索先のIndex
		const uint32_t index = source.index_;

		// 取得するアニメーションのデータ
		const auto *const animData = animationAssimp[index];

		// アニメーションの周波数
		const double tickPerSecond = animData->mTicksPerSecond;

		// 時間の単位を秒単位に変更
		result->duration_ = static_cast<float>(animData->mDuration / tickPerSecond);
		// mTicksPerSecond	: 周波数｡ 単位はHz｡
		// mDuration		: mTicksPerSecondで指定された周波数における長さ

		// assimpでは個々のAnimationをchannelと呼んでいるので､channelを回してNodeAnimationの情報を取ってくる｡
		{
			// チャンネルの配列
			const std::span<aiNodeAnim *> aiChannels = std::span<aiNodeAnim *>{ animData->mChannels, animData->mNumChannels };

			for (const aiNodeAnim *nodeAnimationAssimp : aiChannels) {
				// アニメーションの名前から､紐づいた保存先を作成
				NodeAnimation &nodeAnimation = result->nodeAnimations_[nodeAnimationAssimp->mNodeName.C_Str()];

				// 座標データを取得していく
				{
					// 座標データの配列
					const std::span<const aiVectorKey> aiPosKeys = { nodeAnimationAssimp->mPositionKeys, nodeAnimationAssimp->mNumPositionKeys };
					// サイズを拡張
					nodeAnimation.translate_.resize(aiPosKeys.size());

					std::transform(std::execution::unseq, aiPosKeys.begin(), aiPosKeys.end(), nodeAnimation.translate_.begin(), [tickPerSecond](const aiVectorKey &source)->KeyFlameVector3
						{
							KeyFlameVector3 result;
							result.time_ = static_cast<float>(source.mTime / tickPerSecond); // 周波数から秒単位に変換
							result.kValue_ = Vector3{ -source.mValue.x, source.mValue.y, source.mValue.z };  // 右手から左手に変更

							return std::move(result);
						}
					);
				}

				// 姿勢データを取得していく
				{
					// 姿勢データの配列
					const std::span<const aiQuatKey> aiRotKeys = { nodeAnimationAssimp->mRotationKeys, nodeAnimationAssimp->mNumRotationKeys };
					// サイズを拡張
					nodeAnimation.rotate_.resize(aiRotKeys.size());

					std::transform(std::execution::unseq, aiRotKeys.begin(), aiRotKeys.end(), nodeAnimation.rotate_.begin(), [tickPerSecond](const aiQuatKey &source)->KeyFlameQuaternion
						{
							KeyFlameQuaternion result;
							result.time_ = static_cast<float>(source.mTime / tickPerSecond);                        // 周波数から秒単位に変換
							result.kValue_ = Quaternion{ source.mValue.x, -source.mValue.y, -source.mValue.z, source.mValue.w }; // 右手から左手に変更

							return std::move(result);
						}
					);
				}

				// スケールデータを取得していく
				{
					// スケールデータの配列
					const std::span<const aiVectorKey> aiScaleKeys = { nodeAnimationAssimp->mScalingKeys, nodeAnimationAssimp->mNumScalingKeys };
					// サイズを拡張
					nodeAnimation.scale_.resize(aiScaleKeys.size());

					std::transform(std::execution::unseq, aiScaleKeys.begin(), aiScaleKeys.end(), nodeAnimation.scale_.begin(), [tickPerSecond](const aiVectorKey &source)->KeyFlameVector3
						{
							KeyFlameVector3 result;
							result.time_ = static_cast<float>(source.mTime / tickPerSecond); // 周波数から秒単位に変換
							result.kValue_ = Vector3{ source.mValue.x, source.mValue.y, source.mValue.z };   // そのまま代入する

							return std::move(result);
						}
					);
				}
			}
		}

		// データを渡す
		return std::move(result);

	}

}