#pragma once

#define DIRECTINPIT_VERSION 0x0800	// DirectInputのバージョン
#include <dinput.h>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include <wrl.h>

#include <array>
#include <stdint.h>
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