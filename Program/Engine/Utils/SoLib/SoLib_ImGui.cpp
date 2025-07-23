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

bool SoLib::ImGuiDragEuler(const char *const label, float *const radEuler)
{
	// ウィンドウを取得
	ImGuiWindow *window = ImGui::GetCurrentWindow();
	// ウィンドウがスキップされているなら飛ばす
	if (window->SkipItems) { return false; }

	// ImGuiのコンテキストを取得
	ImGuiContext &g = *GImGui;
	// 値が変更されているかのフラグ
	bool value_changed = false;
	// グループの開始(レイアウトの整合性を保つ)
	ImGui::BeginGroup();
	// ラベル名をIDとして使用
	ImGui::PushID(label);
	// 書き込むアイテムの幅を3つに指定
	ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
	
	for (int i = 0; i < 3; i++) {
		// ラベルのIDにインデックスを追加(重複対策)
		ImGui::PushID(i);
		// 値が0以外であれば､行間に隙間を空ける
		if (i > 0) { ImGui::SameLine(0, g.Style.ItemInnerSpacing.x); }
		// 数値を-180から180の範囲でスライダーで変更できるようにする｡
		value_changed |= ImGui::SliderAngle("", &radEuler[i], -180.f, 180.f);
		// 一時的に使用したIDをポップする
		ImGui::PopID();
		// 幅設定を戻す
		ImGui::PopItemWidth();
	}
	// ラベルのIDを破棄する
	ImGui::PopID();

	// ラベルの終端を見つける
	const char *label_end = ImGui::FindRenderedTextEnd(label);
	// ラベルが終端に達していない場合、ラベルを表示する
	if (label != label_end) {
		// 余白を空けてからラベルを表示
		ImGui::SameLine(0, g.Style.ItemInnerSpacing.x);
		ImGui::TextEx(label, label_end);
	}
	// グループ化を終了する
	ImGui::EndGroup();
	return value_changed;
}
