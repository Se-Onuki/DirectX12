#include "ImGuiManager.h"
#include "../../externals/imgui/imgui_impl_dx12.h"
#include "../../externals/imgui/imgui_impl_win32.h"

void ImGuiManager::StartFlame() {

#pragma region ImGuiに新規フレームであると伝える

	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

#pragma endregion

}

void ImGuiManager::StaticInit(const HWND &hwnd, ID3D12Device *const device, uint32_t backBufferCount, ID3D12DescriptorHeap *const srvHeap) {

	//ImGuiの初期化
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX12_Init(device,
		backBufferCount,
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		srvHeap,
		srvHeap->GetCPUDescriptorHandleForHeapStart(),
		srvHeap->GetGPUDescriptorHandleForHeapStart()
	);
}

void ImGuiManager::CreateCommand()
{

	ImGui::Render();

}

void ImGuiManager::Draw(ID3D12GraphicsCommandList *const commandList)
{

	// 実際のCommandListにImGuiの描画コマンドを積む
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);

}

void ImGuiManager::Finalize()
{
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

}
