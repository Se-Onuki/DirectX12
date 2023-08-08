#pragma once

#define DIRECTINPIT_VERSION 0x0800	// DirectInputのバージョン
#include <dinput.h>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include <wrl.h>

class Input
{
public:
	Input() = default;
	~Input() = default;

private:

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

	IDirectInput8 *directInput_ = nullptr;
	IDirectInputDevice8 *keyboard_ = nullptr;
	BYTE key_[256] = {};
	BYTE preKey_[256] = {};
};