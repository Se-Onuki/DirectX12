/// @file NewImGui.h
/// @brief ImGuiのラッピング
/// @author ONUKI seiya
#pragma once
#include "SoLib_Traits.h"
#include "../Success.h"


namespace SoLib {
	namespace NewImGui {

		template<IsRealType T>
		IsSuccess ImGuiWidget(const T *const value);

		IsSuccess ImGuiWidget(const std::string_view &value);

		template<IsRealType T>
		IsSuccess ImGuiWidget(const std::string_view &label, T *const value);

		template<IsRealType T>
		IsSuccess ImGuiWidget(const std::string_view &label, const T *const value);

	}
}