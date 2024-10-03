#pragma once
#include "../../Engine/DirectBase/Base/TextureManager.h"	// テクスチャのマネージャ
#include "../../Engine/Utils/Graphics/Color.h"						// カラー型

/// @brief 改善版スプライト
class NewSprite {
public:
	NewSprite() = default;
	~NewSprite() = default;


private:
	// テクスチャ
	TextureHandle texture_;
	// 色
	SoLib::Color::RGB4 color_;

	Transform2D transform_;

};
