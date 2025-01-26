/// @file NumberRender.h
/// @brief 数の描画クラス
/// @author ONUKI seiya
#pragma once

#include <list>
#include <memory>
#include "../../DirectBase/2D/Sprite.h"

class NumberText {
public:
	constexpr static size_t kNumCount_ = 3u;

	static void Generate(NumberText *const num, TextureHandle texture);
	static std::unique_ptr<NumberText> Generate(TextureHandle texture);

	void SetText(uint32_t text);

private:
	// 文字データ
	std::array<std::unique_ptr<Sprite>, kNumCount_> numText_;
	// 文字サイズ
	Vector2 textSize_;
	// 文字数
	uint32_t textCount_;
	
};

class NumberRender {
public:

	static std::unique_ptr<NumberRender> Generate(Vector2 texSize, TextureHandle texture);


private:

	std::span<std::unique_ptr<NumberText>> EmplaceBack(size_t count = 1);


	std::vector<std::unique_ptr<NumberText>> text_;
	uint32_t index_ = 0u;

	TextureHandle texture_;


};
