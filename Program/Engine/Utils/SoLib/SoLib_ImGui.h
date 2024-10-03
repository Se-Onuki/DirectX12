#pragma once
#include <stdint.h>
#include <imgui.h>

#include "SoLib_Traits.h"	// 型情報など

#include "../Math/Vector2.h"
#include "../Math/Vector3.h"
#include "../Math/Vector4.h"

#include "../Math/Angle.h"
#include "../Math/ValueRange.h"

#include "../../Engine/DirectBase/File/VariantItem.h"
#include <string>

namespace SoLib {

	// ImGuiの前方宣言
	template<IsNotPointer T>
	bool ImGuiWidget(const char *const label, T *const value) {
		// ImGuiWidget関数を呼び出す
		return value->ImGuiWidget(label);
	}

	template<IsNotPointer T>
	void ImGuiText(const char *const label, const T &text);

	template<>
	void ImGuiText(const char *const label, const std::string &text);

	template <>
	bool ImGuiWidget<bool>(const char *const label, bool *const value);

	template <>
	bool ImGuiWidget<int32_t>(const char *const label, int32_t *const value);
	template <>
	bool ImGuiWidget<float>(const char *const label, float *const value);
	template <>
	bool ImGuiWidget<double>(const char *const label, double *const value);
	template <>
	bool ImGuiWidget<Vector2>(const char *const label, Vector2 *const value);
	template <>
	bool ImGuiWidget<Vector3>(const char *const label, Vector3 *const value);
	template <>
	bool ImGuiWidget<Vector4>(const char *const label, Vector4 *const value);

	template <>
	bool ImGuiWidget<std::string>(const char *const label, std::string *const value);

	template <>
	bool ImGuiWidget<Angle::Radian>(const char *const label, Angle::Radian *const value);

	//template <typename T, IsContainsType<T> C>
	//bool ImGuiWidget(const char *const label, C *const value, uint32_t &index);

	template <SoLib::IsContainer C>
	uint32_t ImGuiWidget(const char *const label, const C *const value, const uint32_t index, const std::function<std::string(uint32_t)> &displayChar = [](uint32_t i) { return std::to_string(i); });

	template <SoLib::IsContainer C, SoLib::IsIterator Itr = decltype(std::begin(std::declval<C>())), SoLib::IsFunction<std::string, const Itr &> Func = std::function<std::string(const Itr &)>>
	Itr ImGuiWidget(const char *const label, C *const value, Itr itr, const Func &displayChar);

	bool ImGuiWidgetAngle(const char *const label, float *const value, float min = -360.f, float max = +360.f);

	template <typename ITEM, SoLib::Text::ConstExprString V = ITEM::str_, SoLib::IsNotPointer T = typename ITEM::ItemType>
		requires(std::same_as<ITEM, VariantItem<V, T>>)
	inline bool ImGuiWidget(ITEM *const value) { return SoLib::ImGuiWidget(value->GetKey(), &value->GetItem()); }

	template <typename T>
	inline bool ImGuiWidget(const char *const label, ValueRange<T> *const value);

	bool ImGuiDragEuler(const char *const label, float *value);

}

#pragma region inline関数の記述

template<typename T>
bool SoLib::ImGuiWidget(const char *const label, ValueRange<T> *const value) {
	bool isAction = false;

#ifdef USE_IMGUI

	if (ImGui::TreeNode(label)) {
		isAction |= SoLib::ImGuiWidget("min", &(value->min_));
		isAction |= SoLib::ImGuiWidget("max", &(value->max_));
		ImGui::TreePop();
	}
#else
	label; value;

#endif // USE_IMGUI

	return isAction;
}

template<SoLib::IsContainer C>
uint32_t SoLib::ImGuiWidget(const char *const label, const C *const value, const uint32_t index, const std::function<std::string(uint32_t)> &displayChar) {

	//T *selectItem = nullptr;
	uint32_t result = index;

#ifdef USE_IMGUI


	if (ImGui::BeginCombo(label, displayChar(index).c_str())) {
		for (uint32_t i = 0u; i < value->size(); i++) {
			bool is_selected = (index == i);

			std::string itemName = displayChar(i);

			// もし空文字列なら表示しない
			if (itemName != "") {

				if (ImGui::Selectable(itemName.c_str(), is_selected)) {
					result = i;
				}
			}
			/*	if (is_selected) {
					ImGui::SetItemDefaultFocus();
				}*/
		}
		ImGui::EndCombo();
	}

#else
	label; value; displayChar;

#endif // USE_IMGUI

	return result;
}

template<SoLib::IsContainer C, SoLib::IsIterator Itr, SoLib::IsFunction<std::string, const Itr &> Func>
Itr SoLib::ImGuiWidget(const char *const label, C *const value, Itr itr, const Func &displayChar) {

	// イテレータが無効であった場合に初期化する
	if (itr == value->end()) {
		// 初期化
		itr = value->begin();
	}
	// 表示文字列
	std::string previewName;

	// コンテナが空でなければ
	if (value->size()) {
		// 文字列を代入
		previewName = displayChar(itr);
	}

	if (ImGui::BeginCombo(label, previewName.c_str())) {
		for (Itr i = value->begin(); i != value->end(); ++i) {
			//bool is_selected = (itr == i);

			// アイテムの文字列
			std::string itemName = displayChar(i);

			// もし空文字列なら表示しない
			if (itemName != "") {

				if (ImGui::Selectable(itemName.c_str()/*, is_selected*/)) {
					itr = i;
				}
			}
			/*	if (is_selected) {
					ImGui::SetItemDefaultFocus();
				}*/
		}
		ImGui::EndCombo();
	}

	return itr;
}

#pragma endregion
