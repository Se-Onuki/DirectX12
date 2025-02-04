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
		if (count + index_ == 0) {
			return {};
		}

		// もし範囲外なら
		if (count + index_ > text_.size()) {
			// 容量を確保する
			EmplaceBack(count + index_ - text_.size());
		}

		std::span<std::unique_ptr<NumberText>> result = std::span{ &text_.at(index_), count };
		// 加算する
		index_ += count;

		return result;
	}

	void NumberRender::Draw() const
	{
		for (uint32_t i = 0; i < index_; i++) {
			text_[i]->Draw();
		}
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
		std::generate(begin, text_.end(), [this]()->std::unique_ptr<NumberText>
			{
				auto itm = NumberText::Generate(texture_);
				itm->SetPivot(Vector2::one / 2);
				itm->SetScale(0.25f);
				return std::move(itm);
			}
		);

		return std::span(begin, count);
	}

	void NumberText::Generate(NumberText *const numText, TextureHandle texture)
	{
		numText->textSize_ = texture->textureSize_;
		numText->textSize_.x /= 10.f;

		auto textureSize = numText->textSize_;

		std::generate(numText->numText_.begin(), numText->numText_.end(), [texture, textureSize]()->std::unique_ptr<Sprite> {
			auto itm = Sprite::Generate(texture.index_);
			itm->SetPivot(Vector2::one * 0.5f);
			itm->SetTexDiff(textureSize);
			itm->SetScale(textureSize);
			return std::move(itm);
			}
		);

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
		if (textCount_ == 0) { return; }

		for (uint32_t i = 0; i < (std::min)(textCount_, static_cast<uint32_t>(kNumCount_)); i++) {
			numText_[i]->SetTexOrigin(Vector2{ textXSize * static_cast<float>(str[textCount_ - i - 1] - '0'), 0.f });
		}
	}

	void NumberText::SetPosition([[maybe_unused]] Vector2 pos)
	{
		Vector2 beginPos = { pos.x - (textSize_.x * ((kNumCount_ - 1.5f) * textMul_ / 2)), pos.y - ((textSize_.y / 2) + textSize_.y * pivot_.y) * textMul_ };
		Vector2 diff = Vector2{ textSize_.x * (kNumCount_ - 1) * textMul_, 0.f } / kNumCount_;

		for (auto itr = numText_.rbegin(); itr != numText_.rend(); itr++) {
			(*itr)->SetPosition(beginPos);
			beginPos += diff;
		}

	}

	void NumberText::SetPivot(Vector2 pivot)
	{
		pivot_ = pivot;
	}
	void NumberText::SetScale(float scale)
	{
		textMul_ = scale;
		for (auto &itm : numText_) { itm->SetScale(textSize_ * scale); }
	}
	void NumberText::Draw() const
	{
		for (const auto &itm : numText_) {
			itm->Draw();
		}
	}
}