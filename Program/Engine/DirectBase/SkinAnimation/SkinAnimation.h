/// @file SkinAnimation.h
/// @brief スキニングのアニメーション
/// @author ONUKI seiya
#pragma once
#include "../../Engine/Utils/SoLib/SoLib_Timer.h"
#include <unordered_map>
#include "../Model/AssimpData.h"
#include "../Model/ModelData.h"
#include "../Model/SkeletonAnimation/ModelAnimation.h"

namespace SolEngine {
	class AnimationPlayer {
	public:
		AnimationPlayer() = default;
		AnimationPlayer(const ResourceHandle<Animation> animation) : animation_(animation) {}

		/// @brief アニメーションを設定
		/// @param[in] animation アニメーション
		inline void SetAnimation(const ResourceHandle<Animation> animation) { animation_ = animation; }

		/// @brief アニメーションを取得
		/// @return アニメーション
		const ResourceHandle<Animation> &GetAnimation() const { return animation_; }
		/// @brief アニメーションを再生する
		/// @param[in] isLoop true:ループ再生する false:ループ再生しない
		void Start(bool isLoop = false);

		/// @brief アニメーションを更新する
		/// @param[in] deltaTime 経過差分時間
		void Update(float deltaTime);

		/// @brief アニメーションの時刻を取得
		/// @return アニメーションの時刻
		const SoLib::DeltaTimer &GetDeltaTimer() const {
			return animationTimer_;
		}

	private:

		/// @brief 再起的にモデルの姿勢を取得する
		/// @param animateTime アニメーションの時間
		/// @param modelNode モデルのノードの参照
		void CalcTransform(float animateTime, ModelNode *const modelNode) const;

		/// @brief 再起的にモデルの姿勢を取得する
		/// @param animateTime アニメーションの時間
		/// @param modelNode モデルのノードの参照
		//void CalcTransform(float animateTime, SkeletonState *const modelNode) const;

		SoLib::Time::DeltaTimer animationTimer_; // アニメーションの時刻

		ResourceHandle<Animation> animation_;
	};

}