#include "Input.h"
#include <cassert>
#include "../Base/WinApp.h"

void DirectInput::Init() {

	const WinApp *const winApp = WinApp::GetInstance();
	HRESULT hr = S_FALSE;
	// DirectInputの初期化
	hr = DirectInput8Create(
		winApp->GetHInstance(), DIRECTINPIT_VERSION, IID_IDirectInput8, (void **)&directInput_, nullptr
	);
	assert(SUCCEEDED(hr));

	// キーボードデバイスの生成
	hr = directInput_->CreateDevice(GUID_SysKeyboard, &keyboard_, NULL);
	assert(SUCCEEDED(hr));

	// 入力データ形式のセット
	hr = keyboard_->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(hr));

	// 排他制御レベルのセット
	hr = keyboard_->SetCooperativeLevel(
		winApp->GetHWND(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY
	);
	assert(SUCCEEDED(hr));
}

void DirectInput::Update() {

	inputTarget = !inputTarget;
	keyboard_->Acquire();
	keyboard_->GetDeviceState(sizeof(key_[inputTarget]), &key_[inputTarget]);
}
