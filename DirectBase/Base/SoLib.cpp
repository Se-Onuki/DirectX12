#include "SoLib.h"


#include <imgui.h>

bool SoLib::ImGuiWidget(const char *const label, bool *const value) {
	return ImGui::Checkbox(label, value);
}

bool SoLib::ImGuiWidget(const char *const label, int32_t *const value) {
	return ImGui::InputInt(label, value);
}

bool SoLib::ImGuiWidget(const char *const label, float *const value) {
	return ImGui::DragFloat(label, value, 0.1f, 0.f, 100.f);
}

bool SoLib::ImGuiWidget(const char *const label, double *const value) {
	return ImGui::InputDouble(label, value, 0.1f, 1.f);
}

bool SoLib::ImGuiWidget(const char *const label, Vector2 *const value) {
	return ImGui::DragFloat2(label, &value->x, 0.1f, 0.f, 100.f);
}

bool SoLib::ImGuiWidget(const char *const label, Vector3 *const value) {
	return ImGui::DragFloat3(label, &value->x, 0.1f, 0.f, 100.f);
}

bool SoLib::ImGuiWidget(const char *const label, Vector4 *const value) {
	return ImGui::DragFloat4(label, &value->x, 0.1f, 0.f, 100.f);
}

bool SoLib::ImGuiWidget(const char *const label, std::string *const value) {
	ImGui::Text("\"%s\" : \"%s\"", label, value->c_str());
	return false;
}
