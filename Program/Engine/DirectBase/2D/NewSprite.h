#pragma once
#include "../../Engine/DirectBase/Base/TextureManager.h"	// テクスチャのマネージャ
#include "../../Engine/Utils/Graphics/Color.h"						// カラー型
#include "../../Utils/Math/Transform.h"

/// @brief 改善版スプライト
class NewSprite {
public:
	// ブレンドモード
	enum class BlendMode : uint32_t {
		kNone,		// ブレンドなし
		kNormal,	// アルファブレンド
		kAdd,		// 加算合成
		kSub,		// 減算合成
		kMul,		// 乗算合成
		kScreen,	// スクリーン合成

		kCount,
	};

public:
	NewSprite() = default;
	~NewSprite() = default;


private:
	// テクスチャ
	TextureHandle texture_;
	// ブレンドモード
	BlendMode blendMode_ = BlendMode::kNormal;
	// 色
	SoLib::Color::RGB4 color_;

	// スプライトの姿勢
	Transform2D transform_;
	// スプライトのUV座標
	Transform2D uvTransform_{ .scale_ = Vector2::one };

};
