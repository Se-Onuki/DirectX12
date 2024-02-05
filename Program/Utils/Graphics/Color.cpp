#include "Color.h"

const SoLib::Color::RGB4 kWhite = 0xFFFFFFFF;
const SoLib::Color::RGB4 kBlack = 0x00000000;
const SoLib::Color::RGB4 kRed = 0xFF000000;
const SoLib::Color::RGB4 kGreen = 0x00FF0000;
const SoLib::Color::RGB4 kBlue = 0x0000FF00;

SoLib::Color::RGB4::RGB4(const std::array<float, 4u> &color) {
	std::memcpy(this->data(), color.data(), sizeof(RGB4));
	Clamp();
}

SoLib::Color::RGB4::RGB4(const std::array<uint8_t, 4u> color) {
	for (uint8_t i = 0u; i < 4u; i++) {
		this->data()[i] = color[i] / 255.f;
	}
}

SoLib::Color::RGB4::RGB4(const uint32_t color) {

	for (uint8_t i = 0; i < 4; ++i) {
		this->data()[i] = static_cast<float>((color >> (8u * (3u - i))) & 0xFF) / 0xFF;
	}
}

SoLib::Color::RGB4 &SoLib::Color::RGB4::operator=(const uint32_t color) {
	return *this = RGB4(color);
}

SoLib::Color::RGB4 &SoLib::Color::RGB4::operator=(const Vector4 &color) {
	return *this = RGB4(color);
}

SoLib::Color::RGB4::operator uint32_t() const {
	uint32_t result{};

	for (uint8_t i = 0u; i < 4u; ++i) {
		result |= static_cast<uint8_t>(this->data()[3u - i] * 255.f);

		if (i != 3u) { result <<= 8u; }
	}
	return result;
}

SoLib::Color::RGB4::operator std::array<uint8_t, 4u>() const {
	std::array<uint8_t, 4u> result;
	for (uint8_t i = 0u; i < 4u; i++) {
		result[i] = static_cast<uint8_t>(this->data()[i] * 255.f);
	}
	return result;
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

SoLib::Color::RGB4 SoLib::Color::operator+(const RGB4 &a, const RGB4 &b) {
	RGB4 result;

	for (uint8_t i = 0u; i < 4; i++) {
		result.data()[i] = a.data()[i] + b.data()[i];
	}

	return result;
}
SoLib::Color::RGB4 SoLib::Color::operator-(const RGB4 &a, const RGB4 &b) {
	RGB4 result;

	for (uint8_t i = 0u; i < 4; i++) {
		result.data()[i] = a.data()[i] - b.data()[i];
	}

	return result;
}
SoLib::Color::RGB4 SoLib::Color::operator*(const RGB4 &a, const float b) {
	RGB4 result;

	for (uint8_t i = 0u; i < 4; i++) {
		result.data()[i] = a.data()[i] * b;
	}

	return result;
}
void SoLib::Color::to_json(nlohmann::json &json, const RGB4 &color) {
	json = static_cast<std::array<uint8_t, 4u>>(color);
}

void SoLib::Color::from_json(const nlohmann::json &json, RGB4 &color) {
	color = RGB4(json.get<std::array<uint8_t, 4u>>());
}

SoLib::Color::HSV4::HSV4(const uint32_t color) {
	for (uint8_t i = 0; i < 4; ++i) {
		this->data()[i] = static_cast<float>((color >> (8u * (3u - i))) & 0xFF) / 0xFF;
	}
}

SoLib::Color::HSV4 &SoLib::Color::HSV4::operator=(const uint32_t color) {
	return *this = HSV4{ color };
}

SoLib::Color::HSV4 &SoLib::Color::HSV4::operator=(const Vector4 &color) {
	return *this = HSV4{ color };
}
