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

namespace SoLib {

	// ImGuiの前方宣言
	template<IsNotPointer T>
	bool ImGuiWidget(const char *const label, T *const value) {
		// 単体のポインタ型を渡す必要がある
		static_assert(!std::is_pointer<T>::value, "多重ポインタ型が与えられました");
		return false;
	}

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
	bool ImGuiWidget<Angle::Radian>(const char *const label, Angle::Radian *const value);

	bool ImGuiWidgetAngle(const char *const label, float *const value, float min = -360.f, float max = +360.f);

	template<typename T>
	inline bool ImGuiWidget(VariantItem<T> *const value);

	template <typename T>
	inline bool ImGuiWidget(const char *const label, ValueRange<T> *const value);

}

#pragma region inline関数の記述

template<typename T>
bool SoLib::ImGuiWidget(VariantItem<T> *const value) { return SoLib::ImGuiWidget(value->GetKey().c_str(), &value->GetItem()); }

template<typename T>
bool SoLib::ImGuiWidget(const char *const label, ValueRange<T> *const value) {
	bool isAction = false;

#ifdef _DEBUG

	if (ImGui::TreeNode(label)) {
		isAction |= SoLib::ImGuiWidget("min", &(value->min_));
		isAction |= SoLib::ImGuiWidget("max", &(value->max_));
		ImGui::TreePop();
	}
#else
	label; value;

#endif // _DEBUG

	return isAction;
}
#pragma endregion
