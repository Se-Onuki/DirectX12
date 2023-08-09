#pragma once

#define DIRECTINPIT_VERSION 0x0800	// DirectInputのバージョン
#include <dinput.h>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include <array>
#include <stdint.h>
class Input
{
public:
	Input() = default;
	~Input() = default;

private:

};


enum class KeyPad {
	DPAD_UP = 0x0001,
	DPAD_DOWN = 0x0002,
	DPAD_LEFT = 0x0004,
	DPAD_RIGHT = 0x0008,
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

public:


private:

};