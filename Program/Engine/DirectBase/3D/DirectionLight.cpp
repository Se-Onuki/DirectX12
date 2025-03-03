/// @file DirectionLight.cpp
/// @brief 並行光源の実装
/// @author ONUKI seiya
#include "DirectionLight.h"
#include "../Base/DirectXCommon.h"
#include "../Create/Create.h"
#include "../Model/Model.h"
#include "../../Engine/Utils/Math/Math.hpp"

void DirectionLight::Init()
{
	/*lightResource_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(Direction));

	lightResource_->Map(0, nullptr, reinterpret_cast<void **>(&lightData_));*/

	lightData_->color = 0xFFFFFFFF;
	lightData_->direction = Vector3{ 0.f,-1.f,0.f }.Normalize();
	lightData_->intensity = 1.f;
	lightData_->pattern = int32_t(Pattern::kHalfLambert);

	euler_ = Vector3{ 90._deg,0.f,0.f };
}

void DirectionLight::SetLight(ID3D12GraphicsCommandList *const commandList)
{
	commandList->SetGraphicsRootConstantBufferView(static_cast<uint32_t>(Model::RootParameter::kLight), lightData_.GetGPUVirtualAddress());
}

std::unique_ptr<DirectionLight> DirectionLight::Generate()
{
	auto light = std::make_unique<DirectionLight>();
	light->Init();
	return std::move(light);
}

void DirectionLight::ImGuiWidget()
{
	ImGui::ColorEdit3("Color", &lightData_->color.r);
	if (SoLib::ImGuiDragEuler("Direction", &euler_.x)) {
		lightData_->direction = SoLib::EulerToDirection(euler_).Normalize();
	}
	ImGui::DragFloat("Brightness ", &lightData_->intensity, 0.01f, 0, 1);
	const static std::array<const char *const, 3u>lightPattern{ "kNone", "kLambert","kHalfLambert" };
	if (ImGui::BeginCombo("LightingPattren", lightPattern[lightData_->pattern])) {

		for (uint32_t i = 0; i < lightPattern.size(); i++) {
			if (ImGui::Selectable(lightPattern[i])) {
				lightData_->pattern = i;
				break;
			}
		}
		ImGui::EndCombo();
	}
}
