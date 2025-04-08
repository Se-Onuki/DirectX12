#include "ControllerUI.h"
#include "../../Engine/DirectBase/File/GlobalVariables.h"
#include "../../Engine/DirectBase/Input/Input.h"

namespace Game {

	void ControllerUI::Init()
	{

		stickL_ = Sprite::Generate(TextureManager::Load("UI/XboxKey/xbox_stick_top_l.png"));
		stickR_ = Sprite::Generate(TextureManager::Load("UI/XboxKey/xbox_stick_top_r.png"));
		stickLText_ = Sprite::Generate();
		pInput_ = SolEngine::Input::GetInstance();

		SetValue();

	}

	void ControllerUI::Update(float)
	{
#ifdef USE_IMGUI
		SetValue();
#endif // USE_IMGUI

		const auto xInput = pInput_->GetXInput();

		const Vector2 inputL = xInput->GetState()->stickL_;
		const Vector2 inputR = xInput->GetState()->stickR_;

		stickL_->SetPosition(*vStickLPos_ + inputL);
		stickR_->SetPosition(*vStickRPos_ + inputR);

	}

	void ControllerUI::Draw() const
	{
		stickL_->Draw();
		stickR_->Draw();
		stickLText_->Draw();
	}

	void ControllerUI::Save() const
	{
		auto gValue = GlobalVariables::GetInstance();

		auto &group = gValue->GetGroup(kControllerUIGroup_.data());

		group << vStickRUVSize_;
		group << vStickRUV_;

		group << vStickLUVSize_;
		group << vStickLUV_;

		group << vStickRPos_;
		group << vStickLPos_;

	}

	void ControllerUI::Load()
	{
		const auto gValue = GlobalVariables::GetInstance();

		const auto &group = gValue->GetGroup(kControllerUIGroup_.data());

		group >> vStickRUVSize_;
		group >> vStickRUV_;

		group >> vStickLUVSize_;
		group >> vStickLUV_;

		group >> vStickRPos_;
		group >> vStickLPos_;
	}

	bool ControllerUI::ImGuiWidget([[maybe_unused]] const std::string_view &name)
	{
		bool isChange = false;
#ifdef USE_IMGUI

		if (ImGui::TreeNode(name.data())) {
			isChange |= SoLib::ImGuiWidget(&vStickRUV_);
			isChange |= SoLib::ImGuiWidget(&vStickRUVSize_);

			isChange |= SoLib::ImGuiWidget(&vStickLUV_);
			isChange |= SoLib::ImGuiWidget(&vStickLUVSize_);

			ImGui::TreePop();
		}

#endif // USE_IMGUI
		return isChange;
	}

	void ControllerUI::SetValue()
	{

		Load();
		Save();

		stickL_->SetTexOrigin(vStickLUV_);
		stickL_->SetTexDiff(vStickLUVSize_);

		stickR_->SetTexOrigin(vStickRUV_);
		stickR_->SetTexDiff(vStickRUVSize_);
	}

}