#include "DirectionLight.h"
#include "../Base/DirectXCommon.h"
#include "../Create/Create.h"
#include "../Model/Model.h"

void DirectionLight::Init()
{
	lightResource_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(Direction));

	lightResource_->Map(0, nullptr, reinterpret_cast<void **>(&lightData_));

	lightData_->color = { 1.f, 1.f, 1.f,1.f };
	lightData_->direction = Vector3{ 0.f,-1.f,0.f }.Nomalize();
	lightData_->intensity = 1.f;
	lightData_->pattern = int32_t(Pattern::kHalfLambert);
}

void DirectionLight::SetLight(ID3D12GraphicsCommandList *const commandList)
{
	commandList->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kLight, lightResource_->GetGPUVirtualAddress());
}

std::unique_ptr<DirectionLight> DirectionLight::Create()
{
	auto light = std::make_unique<DirectionLight>();
	light->Init();
	return std::move(light);
}

void DirectionLight::ImGuiWidget()
{
	ImGui::ColorEdit3("Color", &lightData_->color.x);
	if (ImGui::DragFloat3("Direction", &lightData_->direction.x, 1.f / 255, -1, 1)) {
		lightData_->direction = lightData_->direction.Nomalize();
	}
	ImGui::DragFloat("Brightness ", &lightData_->intensity, 0.01f, 0, 1);
	const static std::array<std::string, 3u>lightPattern{ "kNone", "kLambert","kHalfLambert" };
	if (ImGui::BeginCombo("LightingPattren", lightPattern[lightData_->pattern].c_str())) {

		for (uint32_t i = 0; i < lightPattern.size(); i++) {
			if (ImGui::Selectable(lightPattern[i].c_str())) {
				lightData_->pattern = i;
				break;
			}
		}
		ImGui::EndCombo();
	}
}
