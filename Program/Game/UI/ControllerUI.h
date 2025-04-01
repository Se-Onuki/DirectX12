/// @file ControllerUI.h
/// @brief コントローラーUIの実装
/// @author ONUKI seiya
#pragma once
#include <memory>
#include "../../Engine/DirectBase/2D/Sprite.h"

namespace Game {

	class ControllerUI {
	public:

		ControllerUI() = default;
		~ControllerUI() = default;

		void Init();

		void Update(float deltaTime);

		void Draw() const;

	private:

		std::unique_ptr<Sprite> stickL_ = nullptr;
		std::unique_ptr<Sprite> stickLBase_ = nullptr;

		std::unique_ptr<Sprite> stickR_ = nullptr;
		std::unique_ptr<Sprite> stickRBase_ = nullptr;

	};

}