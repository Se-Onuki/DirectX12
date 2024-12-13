/// @file ImGuiManager.h
/// @brief ImGuiの管理を行うクラス
/// @author ONUKI seiya
#pragma once
#include <wrl.h>

#include <stdint.h>
#include <d3d12.h>

#include <imgui.h>
#include "../Descriptor/DescriptorManager.h"

class DirectXCommon;

/// @class ImGuiManager
/// @brief ImGuiの管理を行うクラス
class ImGuiManager
{
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	ImGuiManager() = default;
	ImGuiManager(const ImGuiManager &) = delete;
	const ImGuiManager &operator=(const ImGuiManager &) = delete;

	~ImGuiManager() = default;

	DirectXCommon *dxCommon = nullptr;

public:

	/// @fn void StartFlame(void)
	/// @brief ImGuiに新規フレームであると伝える
	static void StartFlame();

	/// @fn ImGuiManager *const GetInstance(void)
	/// @brief ImGuiManagerのシングルトンインスタンスを取得
	/// @return ImGuiManagerのインスタンス
	static ImGuiManager *const GetInstance() {
		static ImGuiManager instance{};
		return &instance;
	}

	/// @fn static void StaticInit(const HWND &, ID3D12Device *const, uint32_t, DescHeap<D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV> *const)
	/// @brief ImGuiの初期化
	/// @param[in] hwnd ウィンドウハンドル
	/// @param[in] device DirectX12デバイスへのアクセッサ
	/// @param[in] backBufferCount バックバッファ数
	/// @param[in] srvHeap シェーダーリソースヒープ
	/// @details ImGuiの初期化を行う｡拡張機能を有効化｡
	static void StaticInit(const HWND &hwnd, ID3D12Device *const device, uint32_t backBufferCount, DescHeap<D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV> *const srvHeap);

	/// @fn static void CreateCommand(void)
	/// @brief ImGuiのコマンドを作成
	/// @details ImGuiのDirectXコマンドを作成
	static void CreateCommand();

	/// @fn static void Draw(ID3D12GraphicsCommandList *const)
	/// @brief 作成したImGuiのコマンドを実行
	/// @param[in] commandList コマンドリスト
	static void Draw(ID3D12GraphicsCommandList *const commandList);

	/// @fn static void Finalize(void)
	/// @brief ImGuiの終了処理
	/// @details ImGuiのシングルトンでの終了処理
	static void Finalize();

private:
	DescHeap<D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV>::HeapRange srvHeapRange_;
};