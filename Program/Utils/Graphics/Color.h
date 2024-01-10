#pragma once
#include <stdint.h>
#include <array>
#include "../SoLib/SoLib_ImGui.h"

#include "../SoLib/SoLib_Json.h"
#include "../Math/Vector4.h"
#include "../SoLib/SoLib_Lerp.h"

namespace SoLib {

	namespace Color {

		struct RGB4 {

			RGB4() = default;
			RGB4(const RGB4 &) = default;
			RGB4(RGB4 &&) = default;
			RGB4(const float r, const float g, const float b, const float a) :r(r), g(g), b(b), a(a) { Clamp(); }
			RGB4(const std::array<float, 4u> &color);
			RGB4(const std::array<uint8_t, 4u> color);
			RGB4(const uint32_t color);
			RGB4(const Vector4 &color) {
				std::memcpy(this->data(), color.data(), sizeof(*this));
				Clamp();
			}

			RGB4 &operator=(const RGB4 &) = default;
			RGB4 &operator=(RGB4 &&) = default;
			RGB4 &operator=(const uint32_t color);
			RGB4 &operator=(const Vector4 &color);

			float r, g, b, a;

			// 白
			static const RGB4 kWhite;
			static const RGB4 kBlack;
			static const RGB4 kRed;
			static const RGB4 kGreen;
			static const RGB4 kBlue;

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
			operator uint32_t() const;

			inline operator Vector4 &() { return *(reinterpret_cast<Vector4 *>(this)); }
			inline operator const Vector4 &() const { return  *(reinterpret_cast<const Vector4 *>(this)); }

		private:
			inline void Clamp() {
				for (uint8_t i = 0u; i < 4; i++) {
					this->data()[i] = std::clamp(this->data()[i], 0.f, 1.f);
				}
			}

		};

		RGB4 operator+(const RGB4 &a, const RGB4 &b);
		RGB4 operator-(const RGB4 &a, const RGB4 &b);

		RGB4 operator*(const RGB4 &a, const float b);
		inline RGB4 operator/(const RGB4 &a, const float b) { return a * (1.f / b); }

		void to_json(nlohmann::json &json, const RGB4 &color);
		void from_json(const nlohmann::json &json, RGB4 &color);

		struct HSV4 {
			HSV4() = default;
			HSV4(const HSV4 &) = default;
			HSV4(HSV4 &&) = default;

			HSV4(const float h, const float s, const float v, const float a) :h(h), s(s), v(v), a(a) { Clamp(); }
			//HSV4(const std::array<float, 4u> &color);
			//HSV4(const std::array<uint8_t, 4u> color);
			HSV4(const uint32_t color);
			HSV4(const Vector4 &color) {
				std::memcpy(this->data(), color.data(), sizeof(*this));
				Clamp();
			}

			HSV4 &operator=(const HSV4 &) = default;
			HSV4 &operator=(HSV4 &&) = default;
			HSV4 &operator=(const uint32_t color);
			HSV4 &operator=(const Vector4 &color);
		public:

			float h, s, v, a;

		public:
			static uint32_t size() { return 4u; }

			/// @brief 始点イテレータ
			/// @return イテレータ
			float *const begin() { return &h; }
			const float *const begin() const { return &h; }

			/// @brief 始点イテレータ
			/// @return イテレータ
			const float *const cbegin() const { return &h; }

			/// @brief メモリアクセス
			/// @return 始点アドレス
			float *const data() { return &h; }
			const float *const data() const { return &h; }

			/// @brief 終点イテレータ
			/// @return イテレータ
			float *const end() { return &h + size(); }
			const float *const end() const { return &h + size(); }

			/// @brief 終点イテレータ
			/// @return イテレータ
			const float *const cend() const { return &h + size(); }

			/// @brief floatポインタ への非明示的変換
			inline operator float *const () { return data(); }
			/// @brief floatポインタ への非明示的変換
			inline operator const float *() const { return data(); }

			operator std::array<uint8_t, 4u>() const;
			operator uint32_t() const;

			inline operator Vector4 &() { return *(reinterpret_cast<Vector4 *>(this)); }
			inline operator const Vector4 &() const { return  *(reinterpret_cast<const Vector4 *>(this)); }

		private:
			inline void Clamp() {
				for (uint8_t i = 0u; i < 4; i++) {
					this->data()[i] = std::clamp(this->data()[i], 0.f, 1.f);
				}
			}
		};
	}
}

template<>
bool SoLib::ImGuiWidget(const char *const label, SoLib::Color::RGB4 *const value);

template<>
bool SoLib::ImGuiWidget(const char *const label, SoLib::Color::HSV4 *const value);