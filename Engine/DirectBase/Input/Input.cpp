#include "Input.h"
#include <cassert>
#include "../Base/WinApp.h"
#include <algorithm>

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
//
//VirtualPad Input::GetPadState(const uint8_t padNumber) const {
//	return VirtualPad();
//}

void VirtualPad::operator<<(const _XINPUT_GAMEPAD &xInput) {
	std::memcpy(&button_, &xInput.wButtons, sizeof(WORD));
	triggerL_ = xInput.bLeftTrigger / 255.f;
	triggerR_ = xInput.bRightTrigger / 255.f;

	stickL_ = Vector2{ std::clamp(xInput.sThumbLX / 32767.f, -1.f,1.f),std::clamp(xInput.sThumbLY / 32767.f, -1.f,1.f) };
	stickR_ = Vector2{ std::clamp(xInput.sThumbRX / 32767.f, -1.f,1.f),std::clamp(xInput.sThumbRY / 32767.f, -1.f,1.f) };

	if (stickL_.Length() < 0.21f) { stickL_ = Vector2::zero; }
	if (stickR_.Length() < 0.21f) { stickR_ = Vector2::zero; }
}

void XInput::Update() {

	std::memcpy(&gamePad_[1], &gamePad_[0], sizeof(VirtualPad));

	XINPUT_STATE state{};
	XInputGetState(
		0,
		&state);

	gamePad_[0] << state.Gamepad;
}

void Input::Init() {
	directInput_ = DirectInput::GetInstance();
	directInput_->Init();
	xInput_ = XInput::GetInstance();
}
