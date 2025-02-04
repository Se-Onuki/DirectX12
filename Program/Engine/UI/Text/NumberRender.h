/// @file NumberRender.h
/// @brief 数の描画クラス
/// @author ONUKI seiya
#pragma once

#include <list>
#include <memory>
#include "../../DirectBase/2D/Sprite.h"
namespace SolEngine {

	class NumberText {
	public:
		constexpr static size_t kNumCount_ = 4u;
		/// @brief 生成関数
		/// @param texture[in] テクスチャ
		static std::unique_ptr<NumberText> Generate(TextureHandle texture);
		/// @param num[out] 生成先
		static void Generate(NumberText *const num, TextureHandle texture);

		/// @brief 文字の指定
		/// @param text 文字
		void SetText(uint32_t text);

		/// @brief 場所の設定
		/// @param pos 場所
		void SetPosition(Vector2 pos);

		/// @brief 描画関数
		void Draw() const;

	private:
		// 文字データ
		std::array<std::unique_ptr<Sprite>, kNumCount_> numText_;
		// 文字サイズ
		Vector2 textSize_;
		// 文字数
		uint32_t textCount_;
		// テキストの表示倍率
		float textMul_ = 1.f;

	};

	class NumberRender {
	public:

		/// @brief 生成関数
		/// @param texture テクスチャ
		/// @return 生成したデータ
		static std::unique_ptr<NumberRender> Generate(TextureHandle texture);

		// データの破棄
		void Clear();

		/// @brief 確保関数
		/// @param count 確保数
		/// @return 確保したデータ
		std::span<std::unique_ptr<NumberText>> Reservation(size_t count);

	private:

		/// @brief 生成
		/// @param count 生成数
		/// @return 生成したデータ群
		std::span<std::unique_ptr<NumberText>> EmplaceBack(size_t count = 1);


		std::vector<std::unique_ptr<NumberText>> text_;
		uint32_t index_ = 0u;

		TextureHandle texture_;


	};
}