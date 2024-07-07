#pragma once
#include <cstdint>
#include <array>
#include <ostream>

namespace SoLib {

	template<typename T>
	concept IsCharType = std::same_as<T, char> or std::same_as<T, wchar_t> or std::same_as<T, char8_t> or std::same_as<T, char16_t> or std::same_as<T, char32_t>;

	namespace Text {


		/// @brief コンパイル時定義固定文字列
		/// @tparam Size 文字列長
		template <IsCharType CharT, uint32_t Size>
		struct ConstExprString {
			static constexpr uint32_t length_ = Size - 1;
			std::array<CharT, Size> buf_;

			using CharType = CharT;

			// 文字列リテラルから推論を行うためのコンストラクタ
			consteval ConstExprString(const CharT(&s_literal)[Size]) {
				for (uint32_t i = 0u; i < Size; i++) {
					buf_[i] = s_literal[i];
				}
			}
		};

		/// @brief 静的固定文字列
		/// @tparam Str 文字列
		template <ConstExprString Str>
		struct StaticString {
			static constexpr ConstExprString str_ = Str;
		public:
			/// @brief 文字列のポインタを返す
			/// @return 文字列ポインタ
			inline static constexpr const decltype(Str)::CharType *const c_str() noexcept { return str_.buf_.data(); }
			inline static constexpr friend std::ostream &operator<< (std::ostream &ost, const StaticString str) noexcept { return ost << str.c_str(); }
		};
	}
}