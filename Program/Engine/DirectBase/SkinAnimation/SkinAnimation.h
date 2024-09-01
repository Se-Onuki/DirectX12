#pragma once
#include "../../../Utils/SoLib/SoLib_Timer.h"
#include <unordered_map>
#include "../Model/AssimpData.h"
#include "../Model/ModelData.h"
#include "../Model/SkeletonAnimation/ModelAnimation.h"

namespace SolEngine {
	class AnimationPlayer {
	public:
		AnimationPlayer() = default;
		AnimationPlayer(const ResourceHandle<Animation> animation) : animation_(animation) {}

		inline void SetAnimation(const ResourceHandle<Animation> animation) { animation_ = animation; }

		const ResourceHandle<Animation> &GetAnimation() const { return animation_; }

		void Start(bool isLoop = false);

		void Update(float deltaTime, SkeletonState *model);

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