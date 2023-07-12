#pragma once
#include <wrl.h>

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


};