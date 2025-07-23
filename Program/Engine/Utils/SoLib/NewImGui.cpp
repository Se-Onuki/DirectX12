#include "NewImGui.h"
#include <imgui.h>
#include <imgui_stdlib.h>
#include "../Text/Text.h"

namespace SoLib::NewImGui {

	IsSuccess ImGuiWidget(const std::string_view &value) {

#ifdef USE_IMGUI

		ImGui::Text(value.data());
		if (ImGui::Button("Copy", ImVec2{ 50.f, 0.f })) {
			return Text::CopyClipboard(value);
		}
		return false;

#else

		value; // 何もしない
		return false;

#endif // USE_IMGUI


	}

}