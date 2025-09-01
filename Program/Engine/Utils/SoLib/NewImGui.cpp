#include "NewImGui.h"
#include <imgui.h>
#include <imgui_stdlib.h>
#include "../Text/Text.h"

namespace SoLib::NewImGui {

	IsSuccess ImGuiWidget(const char *const value) { return ImGuiWidget(std::string_view{ value }); }

	IsSuccess ImGuiWidget(const std::string_view &value) {

#ifdef USE_IMGUI

		if (value.empty()) {
			ImGui::Text("");
			return false; // 空文字列は無視
		}
		bool isClicked = false;
		if (ImGui::Selectable(value.data(), &isClicked, ImGuiSelectableFlags_AllowDoubleClick)) {
			if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
				// ダブルクリックされたらtrue
				return true;
			}
		}
		return false;

#else

		value; // 何もしない
		return false;

#endif // USE_IMGUI


	}

	IsSuccess ImGuiWindow::Draw() const
	{
#ifdef USE_IMGUI

		ImGui::Begin(title_.c_str());

		for (const auto &widget : widgets_) {
			if (widget) {
				widget->Draw();
			}
		}


		ImGui::End();

#endif // USE_IMGUI


		return false;
	}

}