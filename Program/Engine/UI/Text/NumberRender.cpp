#include "NumberRender.h"

namespace SolEngine {
	std::unique_ptr<NumberRender> NumberRender::Generate(TextureHandle texture)
	{
		std::unique_ptr<NumberRender> result = std::make_unique<NumberRender>();
		result->texture_ = texture;

		return std::move(result);
	}

	void NumberRender::Clear()
	{
		index_ = 0;
	}

	std::span<std::unique_ptr<NumberText>> NumberRender::Reservation(size_t count)
	{

		// もし範囲外なら
		if (count + index_ > text_.size()) {
			// 容量を確保する
			EmplaceBack(count + index_ - text_.size());
		}

		std::span<std::unique_ptr<NumberText>> result = std::span{ &text_.at(index_), count };
		// 加算する
		count += index_;

		return result;
	}

	std::span<std::unique_ptr<NumberText>> NumberRender::EmplaceBack(size_t count)
	{
		if (count == 0) { return {}; }
		// 元々のサイズ
		size_t beforeSize = text_.size();
		// サイズ変更
		text_.resize(beforeSize + count);
		// 始点を取得する
		auto begin = text_.begin() + beforeSize;
		std::generate(begin, text_.end(), [this]()->std::unique_ptr<NumberText> { return NumberText::Generate(texture_); });

		return std::span(begin, count);
	}

	void NumberText::Generate(NumberText *const numText, TextureHandle texture)
	{
		numText->textSize_ = texture->textureSize_;
		numText->textSize_.x /= 10.f;

		auto textureSize = numText->textSize_;

		std::generate(numText->numText_.begin(), numText->numText_.end(), [texture, textureSize]()->std::unique_ptr<Sprite> { return Sprite::Generate(texture.index_); });

	}

	std::unique_ptr<NumberText> NumberText::Generate(TextureHandle texture)
	{
		std::unique_ptr<NumberText> result = std::make_unique<NumberText>();
		NumberText::Generate(result.get(), texture);
		return std::move(result);
	}

	void NumberText::SetText(uint32_t text)
	{
		float textXSize = textSize_.x;
		auto str = std::to_string(text);

		textCount_ = static_cast<uint32_t>(str.size());

		for (int32_t i = static_cast<int32_t>((std::min)(str.size(), 3llu) - 1); i >= 0; i--) {
			numText_[i]->SetTexOrigin(Vector2{ 0.f, textXSize * static_cast<float>(str[i] - '0') });
		}
	}

	void NumberText::SetPosition([[maybe_unused]] Vector2 pos)
	{
		/*
		for (uint32_t i = 0; i < textCount_; i++) {

		}*/

	}
}