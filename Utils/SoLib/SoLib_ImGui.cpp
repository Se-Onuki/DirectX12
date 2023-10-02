#include "SoLib_ImGui.h"
#include <imgui.h>

template<>
bool SoLib::ImGuiWidget<bool>(const char *const label, bool *const value) {
#ifdef _DEBUG
	return ImGui::Checkbox(label, value);

#else
	label;	value;
	return false;
#endif // _DEBUG

}

template<>
bool SoLib::ImGuiWidget<int32_t>(const char *const label, int32_t *const value) {
#ifdef _DEBUG
	return ImGui::InputInt(label, value);
#else
	label;	value;
	return false;
#endif // _DEBUG
}

template<>
bool SoLib::ImGuiWidget<float>(const char *const label, float *const value) {
#ifdef _DEBUG
	return ImGui::DragFloat(label, value, 0.1f, 0.f, 100.f);
#else
	label;	value;
	return false;
#endif // _DEBUG
}

template<>
bool SoLib::ImGuiWidget<double>(const char *const label, double *const value) {
#ifdef _DEBUG
	return ImGui::InputDouble(label, value, 0.1f, 1.f);
#else
	label;	value;
	return false;
#endif // _DEBUG
}

template<>
bool SoLib::ImGuiWidget<Vector2>(const char *const label, Vector2 *const value) {
#ifdef _DEBUG
	return ImGui::DragFloat2(label, &value->x, 0.1f, 0.f, 100.f);
#else
	label;	value;
	return false;
#endif // _DEBUG
}

template<>
bool SoLib::ImGuiWidget<Vector3>(const char *const label, Vector3 *const value) {
#ifdef _DEBUG
	return ImGui::DragFloat3(label, &value->x, 0.1f, 0.f, 100.f);
#else
	label;	value;
	return false;
#endif // _DEBUG
}

template<>
bool SoLib::ImGuiWidget<Vector4>(const char *const label, Vector4 *const value) {
#ifdef _DEBUG
	return ImGui::DragFloat4(label, &value->x, 0.1f, 0.f, 100.f);
#else
	label;	value;
	return false;
#endif // _DEBUG
}

bool SoLib::ImGuiWidgetAngle(const char *const label, float *const value, float min, float max) {
#ifdef _DEBUG
	return ImGui::SliderAngle(label, value, min, max);
#else
	label;	value; min; max;
	return false;
#endif // _DEBUG
}
