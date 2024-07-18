#include "Color.h"

namespace SoLib::Color {

	const RGB4 kWhite = 0xFFFFFFFF;
	const RGB4 kBlack = 0x00000000;
	const RGB4 kRed = 0xFF000000;
	const RGB4 kGreen = 0x00FF0000;
	const RGB4 kBlue = 0x0000FF00;

	RGB4::RGB4(const std::array<float, 4u> &color) {
		std::memcpy(this->data(), color.data(), sizeof(RGB4));
		Clamp();
	}

	RGB4::RGB4(const std::array<uint8_t, 4u> color) {
		std::transform(color.begin(), color.end(), begin(), [](uint8_t c) { return static_cast<float>(c) / 0xFF; });
	}

	RGB4::RGB4(const uint32_t color) {
		const std::array<uint8_t, 4u> arr = reinterpret_cast<const std::array<uint8_t, 4u>&>(color);
		std::transform(arr.crbegin(), arr.crend(), this->begin(), [](uint8_t l) { return static_cast<float>(l) / 0xff; });
	}

	RGB4 &RGB4::operator=(const uint32_t color) {
		return *this = RGB4(color);
	}

	RGB4 &RGB4::operator=(const Vector4 &color) {
		return *this = RGB4(color);
	}

	RGB4::operator uint32_t() const {
		uint32_t result{};
		std::array<uint8_t, 4u> &arr = reinterpret_cast<std::array<uint8_t, 4u>&>(result);
		std::transform(this->begin(), this->end(), arr.rbegin(), [](float l) { return static_cast<uint8_t>(l * 0xff); });
		return result;
	}

	RGB4 operator+(const RGB4 &a, const RGB4 &b) {
		RGB4 result;
		std::transform(a.begin(), a.end(), b.begin(), result.begin(), [](float a, float b) { return a + b; });
		return result;
	}

	RGB4 operator-(const RGB4 &a, const RGB4 &b) {
		RGB4 result;
		std::transform(a.begin(), a.end(), b.begin(), result.begin(), [](float a, float b) { return a - b; });
		return result;
	}

	RGB4 operator*(const RGB4 &a, const float b) {
		RGB4 result;
		std::transform(a.begin(), a.end(), result.begin(), [b](float a) { return a * b; });
		return result;
	}

	void to_json(nlohmann::json &json, const RGB4 &color) {
		std::array<uint8_t, 4u> buff;
		std::transform(color.begin(), color.end(), buff.rbegin(), [](float f) { return static_cast<uint8_t>(f * 0xff); });
		json = buff;
	}

	void from_json(const nlohmann::json &json, RGB4 &color) {
		color = RGB4{ json.get<std::array<uint8_t, 4u>>() };
	}

	HSV4::HSV4(const uint32_t color) {
		const std::array<uint8_t, 4u> arr = reinterpret_cast<const std::array<uint8_t, 4u>&>(color);
		std::transform(arr.crbegin(), arr.crend(), this->begin(), [](uint8_t l) { return static_cast<float>(l) / 0xff; });
	}

	HSV4 &HSV4::operator=(const uint32_t color) {
		return *this = HSV4{ color };
	}

	HSV4 &HSV4::operator=(const Vector4 &color) {
		return *this = HSV4{ color };
	}

	HSV4::operator uint32_t() const
	{
		uint32_t result{};
		std::array<uint8_t, 4u> &arr = reinterpret_cast<std::array<uint8_t, 4u>&>(result);
		std::transform(this->begin(), this->end(), arr.rbegin(), [](float l) { return static_cast<uint8_t>(l * 0xff); });
		return result;
	}

	HSV4 operator+(const HSV4 &a, const HSV4 &b)
	{
		HSV4 result;
		std::transform(a.begin(), a.end(), b.begin(), result.begin(), [](float a, float b) { return a + b; });
		return result;
	}
	HSV4 operator-(const HSV4 &a, const HSV4 &b)
	{
		HSV4 result;
		std::transform(a.begin(), a.end(), b.begin(), result.begin(), [](float a, float b) { return a - b; });
		return result;
	}
	HSV4 operator*(const HSV4 &a, const float b)
	{
		HSV4 result;
		std::transform(a.begin(), a.end(), result.begin(), [b](float a) { return a * b; });
		return result;
	}
	void to_json(nlohmann::json &json, const HSV4 &color)
	{
		std::array<uint8_t, 4u> buff;
		std::transform(color.begin(), color.end(), buff.rbegin(), [](float f) { return static_cast<uint8_t>(f * 0xff); });
		json = buff;
	}
	void from_json(const nlohmann::json &json, HSV4 &color)
	{
		const auto &arr = json.get<std::array<uint8_t, 4u>>();
		std::transform(arr.begin(), arr.end(), color.begin(), [](uint8_t c) { return static_cast<float>(c) / 0xFF; });
	}
}

template<>
bool SoLib::ImGuiWidget([[maybe_unused]] const char *const label, [[maybe_unused]] Color::RGB4 *const value) {
#ifdef USE_IMGUI
	return ImGui::ColorEdit4(label, *value);
#else
	return false;
#endif // USE_IMGUI
}

template<>
bool SoLib::ImGuiWidget([[maybe_unused]] const char *const label, [[maybe_unused]] Color::HSV4 *const value) {
#ifdef USE_IMGUI
	return ImGui::ColorEdit4(label, *value, ImGuiColorEditFlags_DisplayHSV | ImGuiColorEditFlags_InputHSV | ImGuiColorEditFlags_PickerHueWheel);
#else
	return false;
#endif // USE_IMGUI
}