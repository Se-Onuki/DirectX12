#include "Color.h"

const SoLib::Color::RGB4 kWhite = 0xFFFFFFFF;

SoLib::Color::RGB4::RGB4(const std::array<float, 4u> &color) {
	std::memcpy(this->data(), color.data(), RGB4::size());
}

SoLib::Color::RGB4::RGB4(const std::array<uint8_t, 4u> &color) {
	for (uint8_t i = 0u; i < 4u; i++) {
		this->data()[i] = color[i] / 255.f;
	}
}

SoLib::Color::RGB4::RGB4(const uint32_t &color) {

	for (uint8_t i = 0; i < 4; ++i) {
		this->data()[i] = static_cast<float>((color >> (8u * (3u - i))) & 0xFF) / 0xFF;
	}
}

SoLib::Color::RGB4 &SoLib::Color::RGB4::operator=(const uint32_t &color) {
	return *this = RGB4(color);
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
#ifdef _DEBUG
	return ImGui::ColorEdit4(label, *value);
#else
	return false;
#endif // _DEBUG
}
void SoLib::Color::to_json(nlohmann::json &json, const RGB4 &color) {
	json = static_cast<std::array<uint8_t, 4u>>(color);
}

void SoLib::Color::from_json(const nlohmann::json &json, RGB4 &color) {
	color = RGB4(json.get<std::array<uint8_t, 4u>>());
}
