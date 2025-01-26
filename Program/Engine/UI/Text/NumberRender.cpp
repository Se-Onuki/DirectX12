#include "NumberRender.h"

std::unique_ptr<NumberRender> NumberRender::Generate(TextureHandle texture)
{
	std::unique_ptr<NumberRender> result = std::make_unique<NumberRender>();
	result->texture_ = texture;

	return std::move(result);
}

std::span<std::unique_ptr<NumberText>> NumberRender::EmplaceBack(size_t count)
{
	if (count == 0) { return {}; }
	// 元々のサイズ
	size_t beforeSize = text_.size();
	// サイズ変更
	text_.resize(beforeSize + count);
	// 
	auto begin = text_.begin() + beforeSize;
	std::generate(begin, text_.end(), [this]()->std::unique_ptr<NumberText> { return NumberText::Generate(texture_); });

	return std::span<std::unique_ptr<NumberText>>(begin, count);
}

void NumberText::Generate(NumberText *const num, TextureHandle texture)
{
	num->textSize_ = texture->textureSize_;
	num->textSize_.y /= 10.f;

	auto textureSize = num->textSize_;

	std::generate(num->numText_.begin(), num->numText_.end(), [texture, textureSize]()->std::unique_ptr<Sprite> { return Sprite::Generate(texture.index_); });

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

	// std::transform(str.rbegin(), str.rbegin() + (std::min)(str.size(), 3llu), numText_.rbegin(), [textXSize](char c, std::unique_ptr<Sprite> &sprite) ->void { sprite->SetTexOrigin(Vector2{ 0.f, textXSize * static_cast<float>(c - '0') }); });
}
