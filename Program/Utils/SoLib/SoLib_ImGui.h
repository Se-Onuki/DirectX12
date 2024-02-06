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
	uint32_t ImGuiWidget(const char *const label, C *const value, const uint32_t index, const std::function<std::string(uint32_t)> &displayChar = [](uint32_t i) { return std::to_string(i); });

	template <SoLib::IsContainer C, SoLib::IsIterator iterator = typename C::iterator>
	iterator ImGuiWidget(const char *const label, C *const value, const iterator &itr, const std::function<std::string(const iterator &)> &displayChar);

	bool ImGuiWidgetAngle(const char *const label, float *const value, float min = -360.f, float max = +360.f);

	template<typename T>
	inline bool ImGuiWidget(VariantItem<T> *const value);

	template <typename T>
	inline bool ImGuiWidget(const char *const label, ValueRange<T> *const value);

	bool ImGuiDragEuler(const char *const label, float *value);

}

#pragma region inline関数の記述

template<typename T>
bool SoLib::ImGuiWidget(VariantItem<T> *const value) { return SoLib::ImGuiWidget(value->GetKey().c_str(), &value->GetItem()); }

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
uint32_t SoLib::ImGuiWidget(const char *const label, C *const value, const uint32_t index, const std::function<std::string(uint32_t)> &displayChar) {

	//T *selectItem = nullptr;
	uint32_t result = index;

	if (ImGui::BeginCombo((label + std::string("Combo")).c_str(), displayChar(index).c_str())) {
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

	/*auto selectItem = std::next(value->begin(), index);

	if (*selectItem) {
		isChanged |= SoLib::ImGuiWidget(label, &*selectItem);
	}*/

	return result;
}

template<SoLib::IsContainer C, SoLib::IsIterator iterator>
iterator SoLib::ImGuiWidget(const char *const label, C *const value, const iterator &itr, const std::function<std::string(const iterator &)> &displayChar) {

	iterator result = itr;

	if (ImGui::BeginCombo((label + std::string("Combo")).c_str(), displayChar(itr).c_str())) {
		for (iterator i = value->begin(); i != value->end(); ++i) {
			bool is_selected = (itr == i);

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

	return result;
}

#pragma endregion
