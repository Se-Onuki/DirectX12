#pragma once
#include <wrl.h>

#include <stdint.h>
#include <d3d12.h>

#include <imgui.h>
#include "../Descriptor/DescriptorManager.h"

class DirectXCommon;

class ImGuiManager
{
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	ImGuiManager() = default;
	ImGuiManager(const ImGuiManager &) = delete;
	const ImGuiManager &operator=(const ImGuiManager &) = delete;

	~ImGuiManager() = default;

	DirectXCommon *dxCommon = nullptr;

public:

	static void StartFlame();

	static ImGuiManager *const GetInstance() {
		static ImGuiManager instance{};
		return &instance;
	}

	static void StaticInit(const HWND &hwnd, ID3D12Device *const device, uint32_t backBufferCount, DescHeap<D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV> *const srvHeap);

	static void CreateCommand();

	static void Draw(ID3D12GraphicsCommandList *const commandList);

	static void Finalize();
};