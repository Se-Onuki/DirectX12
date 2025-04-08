/// @file ControllerUI.h
/// @brief コントローラーUIの実装
/// @author ONUKI seiya
#pragma once
#include <memory>
#include "../../Engine/DirectBase/2D/Sprite.h"

namespace SolEngine {
	class Input;
}

namespace Game {

	class ControllerUI {
	public:

		ControllerUI() = default;
		~ControllerUI() = default;

		void Init();

		void Update(float deltaTime);

		void Draw() const;

	private:

		void Save() const;
		void Load();
		bool ImGuiWidget(const std::string_view &name);

		void SetValue();

		constexpr static std::string_view kControllerUIGroup_ = "ControllerUI";

	private:

		VItem(Vector2, StickRUV, _) = { {} };
		VItem(Vector2, StickRUVSize, _) = { {} };

		VItem(Vector2, StickLUV, _) = { {} };
		VItem(Vector2, StickLUVSize, _) = { {} };

		VItem(Vector2, StickRPos, _) = {};
		VItem(Vector2, StickLPos, _) = {};


	private:

		SolEngine::Input *pInput_ = nullptr;

		std::unique_ptr<Sprite> stickL_ = nullptr;
		std::unique_ptr<Sprite> stickLBase_ = nullptr;
		std::unique_ptr<Sprite> stickLText_ = nullptr;

		std::unique_ptr<Sprite> stickR_ = nullptr;
		std::unique_ptr<Sprite> stickRBase_ = nullptr;

	};

}