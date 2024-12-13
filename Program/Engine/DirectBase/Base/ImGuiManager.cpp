/// @file ImGuiManager.cpp
/// @brief ImGuiの管理を行うクラス
/// @author ONUKI seiya
#include "ImGuiManager.h"
#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>
#include "imnodes.h"

void ImGuiManager::StartFlame() {

#pragma region ImGuiに新規フレームであると伝える

	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

#pragma endregion

}

void ImGuiManager::StaticInit(const HWND &hwnd, ID3D12Device *const device, uint32_t backBufferCount, DescHeap<D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV> *const srvHeap) {

	GetInstance()->srvHeapRange_ = std::move(srvHeap->RequestHeapAllocation(1u));
	const auto &heapHandle = GetInstance()->srvHeapRange_.GetHandle(0u);

	//ImGuiの初期化
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	auto &imGuiIO = ImGui::GetIO();
	// 日本語対応
	imGuiIO.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\msgothic.ttc", 13.0f, NULL, ImGui::GetIO().Fonts->GetGlyphRangesJapanese());
	// ドッキング機能の有効化
	imGuiIO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	imGuiIO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX12_Init(device,
		backBufferCount,
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		srvHeap->GetHeap(),
		heapHandle.cpuHandle_,
		heapHandle.gpuHandle_
	);

	ImNodes::CreateContext();
}

void ImGuiManager::CreateCommand()
{

	ImGui::Render();

}

void ImGuiManager::Draw(ID3D12GraphicsCommandList *const commandList)
{
	commandList;
#ifdef USE_IMGUI

	// 実際のCommandListにImGuiの描画コマンドを積む
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);

#endif // USE_IMGUI

}

void ImGuiManager::Finalize()
{
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImNodes::DestroyContext();
	ImGui::DestroyContext();

}
