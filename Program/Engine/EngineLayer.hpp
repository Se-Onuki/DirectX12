#pragma once
#include <cstdint>

namespace SolEngine {

	static constexpr uint32_t kLayerCount = 3u;

	enum class EngineComponentLayer {
		kPlatformLayer = (1 << kLayerCount),			// プラットフォーム独立層
		kCoreSystemLayer = (2 << kLayerCount),			// コアシステム層
		kResourceLayer = (3 << kLayerCount),			// リソース､アセット層
		kRendererLayer = (4 << kLayerCount),			// 低レベルレンダラ層
		kDebugLayer,									// デバッグ層
		kCollisionLayer,								// コリジョン､物理層
		kHidLayer,										// 入力層
		kSkeletonAnimationLayer = (5 << kLayerCount),	// スケルトンアニメーション層

	};

	class EngineLayer  {



	};

}