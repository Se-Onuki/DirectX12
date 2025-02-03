/// @file SoLib_ImGui.cpp
/// @brief ImGuiのラッピング
/// @author ONUKI seiya
#include "SoLib_ImGui.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>
#include "../Math/Math.hpp"

template<>
void SoLib::ImGuiDraw([[maybe_unused]] const char *const label, [[maybe_unused]] const std::string &text) {
#ifdef USE_IMGUI
	ImGui::Text("%s : %s", label, text.c_str());
#endif // USE_IMGUI
}

template<>
bool SoLib::ImGuiWidget<bool>([[maybe_unused]] const char *const label, [[maybe_unused]] bool *const value) {
#ifdef USE_IMGUI
	return ImGui::Checkbox(label, value);

#else
	return false;
#endif // USE_IMGUI

}

template<>
bool SoLib::ImGuiWidget<int32_t>([[maybe_unused]] const char *const label, [[maybe_unused]] int32_t *const value) {
#ifdef USE_IMGUI
	return ImGui::InputInt(label, value);
#else
	return false;
#endif // USE_IMGUI
}

template<>
bool SoLib::ImGuiWidget<float>([[maybe_unused]] const char *const label, [[maybe_unused]] float *const value) {
#ifdef USE_IMGUI
	return ImGui::DragFloat(label, value, 0.1f);
#else
	return false;
#endif // USE_IMGUI
}

template<>
bool SoLib::ImGuiWidget<double>([[maybe_unused]] const char *const label, [[maybe_unused]] double *const value) {
#ifdef USE_IMGUI
	return ImGui::InputDouble(label, value, 0.1f);
#else
	return false;
#endif // USE_IMGUI
}

template<>
bool SoLib::ImGuiWidget<Vector2>([[maybe_unused]] const char *const label, [[maybe_unused]] Vector2 *const value) {
#ifdef USE_IMGUI
	return ImGui::DragFloat2(label, &value->x, 0.1f);
#else
	return false;
#endif // USE_IMGUI
}

template<>
bool SoLib::ImGuiWidget<Vector3>([[maybe_unused]] const char *const label, [[maybe_unused]] Vector3 *const value) {
#ifdef USE_IMGUI
	return ImGui::DragFloat3(label, &value->x, 0.1f);
#else
	return false;
#endif // USE_IMGUI
}

template<>
bool SoLib::ImGuiWidget<Vector4>([[maybe_unused]] const char *const label, [[maybe_unused]] Vector4 *const value) {
#ifdef USE_IMGUI
	return ImGui::DragFloat4(label, &value->x, 0.1f);
#else
	return false;
#endif // USE_IMGUI
}

template<>
bool SoLib::ImGuiWidget([[maybe_unused]] const char *const label, [[maybe_unused]] std::string *const value) {
#ifdef USE_IMGUI
	return ImGui::InputText(label, value);
#else
	return false;
#endif // USE_IMGUI
}

template<>
bool SoLib::ImGuiWidget([[maybe_unused]] const char *const label, [[maybe_unused]] Angle::Radian *const value) {
#ifdef USE_IMGUI
	return ImGui::SliderAngle(label, &value->Get(), 0.1f);
#else
	return false;
#endif // USE_IMGUI
}

bool SoLib::ImGuiWidgetAngle([[maybe_unused]] const char *const label, [[maybe_unused]] float *const value, [[maybe_unused]] float min, [[maybe_unused]] float max) {
#ifdef USE_IMGUI
	return ImGui::SliderAngle(label, value, min, max);
#else
	return false;
#endif // USE_IMGUI
}

bool SoLib::ImGuiDragEuler(const char *const label, float *const value)
{
	ImGuiWindow *window = ImGui::GetCurrentWindow();
	if (window->SkipItems) { return false; }

	ImGuiContext &g = *GImGui;
	bool value_changed = false;
	ImGui::BeginGroup();
	ImGui::PushID(label);
	ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
	for (int i = 0; i < 3; i++) {
		ImGui::PushID(i);
		if (i > 0) { ImGui::SameLine(0, g.Style.ItemInnerSpacing.x); }
		value_changed |= ImGui::SliderAngle("", &value[i], -180.f, 180.f);
		ImGui::PopID();
		ImGui::PopItemWidth();
	}
	ImGui::PopID();

	const char *label_end = ImGui::FindRenderedTextEnd(label);
	if (label != label_end) {
		ImGui::SameLine(0, g.Style.ItemInnerSpacing.x);
		ImGui::TextEx(label, label_end);
	}

	ImGui::EndGroup();
	return value_changed;
}
