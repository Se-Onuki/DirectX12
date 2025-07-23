/// @file Text.h
/// @brief テキストに関するユーティリティ
/// @author ONUKI seiya
#pragma once
#include "../Success.h"

namespace SoLib {
	namespace Text {
		/// @brief 文字列をクリップボードにコピーする
		/// @param[in] text コピー元の文字列
		/// @return コピーが成功したか
		IsSuccess CopyClipboard(const std::string_view &text) noexcept;
		/// @brief 文字列をクリップボードにコピーする
		/// @param[in] text コピー元の文字列
		/// @return コピーが成功したか
		IsSuccess CopyClipboard(const std::wstring_view &text) noexcept;

		/// @brief ワイド文字列と文字列の相互変換
		/// @param[in] str 変換する文字列
		/// @return 変換後の文字列
		std::wstring ConvertString(const std::string_view &str);

		std::string ConvertString(const std::wstring_view &str);
	}
}