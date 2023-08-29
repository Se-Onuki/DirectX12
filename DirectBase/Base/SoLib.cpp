#include "SoLib.h"


#include <imgui.h>

void SoLib::ImGuiWidget(const char *const label, int32_t *const value) {
	ImGui::InputInt(label, value);
}

void SoLib::ImGuiWidget(const char *const label, float *const value) {
	ImGui::DragFloat(label, value, 0.1f, 0.f, 100.f);
}

void SoLib::ImGuiWidget(const char *const label, Vector2 *const value) {
	ImGui::DragFloat2(label, &value->x, 0.1f, 0.f, 100.f);
}

void SoLib::ImGuiWidget(const char *const label, Vector3 *const value) {
	ImGui::DragFloat3(label, &value->x, 0.1f, 0.f, 100.f);
}

void SoLib::ImGuiWidget(const char *const label, Vector4 *const value) {
	ImGui::DragFloat4(label, &value->x, 0.1f, 0.f, 100.f);
}
