#pragma once
#include <stdint.h>
#include <array>
#include "../SoLib/SoLib_ImGui.h"

#include "../SoLib/SoLib_Json.h"

namespace SoLib {

	namespace Color {

		struct RGB4 {

			RGB4() = default;
			RGB4(const RGB4 &) = default;
			RGB4(RGB4 &&) = default;
			RGB4(const std::array<float, 4u> &color);
			RGB4(const std::array<uint8_t, 4u> &color);


			RGB4(const uint32_t &color);

			RGB4 &operator=(const RGB4 &) = default;
			RGB4 &operator=(RGB4 &&) = default;
			RGB4 &operator=(const uint32_t &color);

			float r, g, b, a;

			// 白
			static const RGB4 kWhite;

			static uint32_t size() { return 4u; }

			/// @brief 始点イテレータ
			/// @return イテレータ
			float *const begin() { return &r; }
			const float *const begin() const { return &r; }

			/// @brief 始点イテレータ
			/// @return イテレータ
			const float *const cbegin() const { return &r; }

			/// @brief メモリアクセス
			/// @return 始点アドレス
			float *const data() { return &r; }
			const float *const data() const { return &r; }

			/// @brief 終点イテレータ
			/// @return イテレータ
			float *const end() { return &r + size(); }
			const float *const end() const { return &r + size(); }

			/// @brief 終点イテレータ
			/// @return イテレータ
			const float *const cend() const { return &r + size(); }

			/// @brief floatポインタ への非明示的変換
			inline operator float *const () { return data(); }
			/// @brief floatポインタ への非明示的変換
			inline operator const float *() const { return data(); }

			operator std::array<uint8_t, 4u>() const;

		private:

		};

		void to_json(nlohmann::json &json, const RGB4 &color);
		void from_json(const nlohmann::json &json, RGB4 &color);
	}
}

template<>
bool SoLib::ImGuiWidget(const char *const label, SoLib::Color::RGB4 *const value);