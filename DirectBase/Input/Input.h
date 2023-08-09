#pragma once

#define DIRECTINPIT_VERSION 0x0800	// DirectInputのバージョン
#include <dinput.h>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include <Windows.h>
#include <Xinput.h>

#pragma comment (lib, "xinput.lib")

#include <array>
#include <stdint.h>
#include "../../Header/Math/Vector2.h"

class DirectInput;

enum class KeyCode {
	DPAD_UP = 0x0001,	// 十字上
	DPAD_DOWN = 0x0002,	// 十字下
	DPAD_LEFT = 0x0004,	// 十字左
	DPAD_RIGHT = 0x0008,// 十字右
	START = 0x0010,
	BACK = 0x0020,
	LEFT_THUMB = 0x0040,
	RIGHT_THUMB = 0x0080,
	LEFT_SHOULDER = 0x0100,
	RIGHT_SHOULDER = 0x0200,
	A = 0x1000,
	B = 0x2000,
	X = 0x4000,
	Y = 0x8000,
};

struct VirtualPad {
	Vector2 stickR_;
	Vector2 stickL_;
	WORD button_;
	float triggerR_;
	float triggerL_;

	void operator<< (const _XINPUT_GAMEPAD &xInput);
};

class DirectInput
{
	DirectInput() = default;
	DirectInput(const DirectInput &) = delete;
	DirectInput operator=(const DirectInput &) = delete;
	~DirectInput() = default;

public:
	static DirectInput *const GetInstance() {
		static DirectInput instance{};
		return &instance;
	}

	void Init();
	void Update();
private:
	IDirectInput8 *directInput_ = nullptr;
	IDirectInputDevice8 *keyboard_ = nullptr;

	bool inputTarget = 0u;
	std::array<BYTE, 256u> key_[2u] = {};

public:
	bool IsPress(const uint8_t key) const {
		return key_[inputTarget][key];
	}
	bool IsTrigger(const uint8_t key) const {
		return key_[inputTarget][key] && !key_[!inputTarget][key];
	}
	bool IsRelease(const uint8_t key) const {
		return key_[!inputTarget][key] && !key_[inputTarget][key];
	}

};
class XInput
{
	XInput() = default;
	XInput(const XInput &) = delete;
	XInput operator=(const XInput &) = delete;
	~XInput() = default;

	std::array<VirtualPad, 2u> gamePad_{};

public:
	static XInput *const GetInstance() {
		static XInput instance{};
		return &instance;
	}

	void Update();

	bool IsPress(const KeyCode keyCode) const {
		return gamePad_[0].button_ & (WORD)keyCode;
	}
	bool IsTrigger(const KeyCode keyCode) const {
		return (gamePad_[0].button_ & (WORD)keyCode) && !(gamePad_[1].button_ & (WORD)keyCode);
	}
	bool IsRelease(const KeyCode keyCode) const {
		return !(gamePad_[0].button_ & (WORD)keyCode) && (gamePad_[1].button_ & (WORD)keyCode);
	}

	const VirtualPad *const GetState() const {
		return &gamePad_[0];
	}

private:

};

class Input
{
	Input() = default;
	Input(const Input &) = delete;
	Input operator=(const Input &) = delete;
	~Input() = default;
public:
	static Input *const GetInstance() {
		static Input instance{};
		return &instance;
	}
	void Init();
	void Update() {
		directInput_->Update();
		xInput_->Update();
	}

	const DirectInput *const GetDirectInput() const {
		return directInput_;
	}
	const XInput *const GetXInput() const {
		return xInput_;
	}


private:
	DirectInput *directInput_ = nullptr;
	XInput *xInput_ = nullptr;
};
