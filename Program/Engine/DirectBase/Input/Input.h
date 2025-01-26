/// @file Input.h
/// @brief 入力管理クラス
/// @author ONUKI seiya
#pragma once
#include <wrl.h>

#define DIRECTINPIT_VERSION 0x0800 // DirectInputのバージョン
#include <dinput.h>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include <Windows.h>
#include <Xinput.h>

#pragma comment(lib, "xinput.lib")

#include "../../Engine/Utils/Math/Vector2.h"
#include <algorithm>
#include <array>
#include <stdint.h>

/// @enum KeyCode
/// @brief キーコード
enum class KeyCode
{
	DPAD_UP = 0x0001,		 // 十字上
	DPAD_DOWN = 0x0002,		 // 十字下
	DPAD_LEFT = 0x0004,		 // 十字左
	DPAD_RIGHT = 0x0008,	 // 十字右
	START = 0x0010,			 // スタートボタン
	BACK = 0x0020,			 // バックボタン
	LEFT_THUMB = 0x0040,	 // 左スティック押下
	RIGHT_THUMB = 0x0080,	 // 右スティック押下
	LEFT_SHOULDER = 0x0100,	 // Lボタン
	RIGHT_SHOULDER = 0x0200, // Rボタン
	A = 0x1000,
	B = 0x2000,
	X = 0x4000,
	Y = 0x8000,
};

/// @struct VirtualPad
/// @brief 仮想パッド
struct VirtualPad
{
	Vector2 stickR_;
	Vector2 stickL_;
	uint16_t button_;
	float triggerR_;
	float triggerL_;

	/// @brief 仮想パッドの情報を入れる
	/// @param[in] xInput XInputの情報
	void operator<<(const _XINPUT_GAMEPAD &xInput);
};

/// @class DirectInput
/// @brief DirectInputの管理
class DirectInput
{
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	DirectInput() = default;
	DirectInput(const DirectInput &) = delete;
	DirectInput operator=(const DirectInput &) = delete;
	~DirectInput() = default;

public:
	static DirectInput *const GetInstance()
	{
		static DirectInput instance{};
		return &instance;
	}

	/// @brief 初期化する
	/// @details DirectInputの取得先などを生成する
	void Init();
	/// @brief 更新する
	void Update();

private:
	ComPtr<IDirectInput8> directInput_ = nullptr;
	ComPtr<IDirectInputDevice8> keyboard_ = nullptr;

	bool inputTarget = 0u;
	std::array<BYTE, 256u> key_[2u] = {};

public:
	/// @brief どれか一つでも押していたら有効
	/// @return 押した場合 true
	bool IsAnyPress() const
	{
		return std::any_of(key_[inputTarget].begin(), key_[inputTarget].end(), [](const auto pair) {
			return pair;
		});
	}
	/// @brief 指定したキーを押しているか
	/// @param[in] key 押されたキー
	/// @return 押した場合 true
	bool IsPress(const uint8_t key) const
	{
		return key_[inputTarget][key];
	}
	/// @brief 指定したキーを押した瞬間か
	/// @param[in] key 押されたキー
	/// @return 押した場合 true
	bool IsTrigger(const uint8_t key) const
	{
		return key_[inputTarget][key] && !key_[!inputTarget][key];
	}
	/// @brief 指定したキーを離しているか
	/// @param[in] key 離したキー
	/// @return 離した場合 true
	bool IsRelease(const uint8_t key) const
	{
		return key_[!inputTarget][key] && !key_[inputTarget][key];
	}
};

/// @class XInput
/// @brief XInputの管理
class XInput
{
	XInput() = default;
	XInput(const XInput &) = delete;
	XInput operator=(const XInput &) = delete;
	~XInput() = default;

	std::array<VirtualPad, 2u> gamePad_{};

public:
	static XInput *const GetInstance()
	{
		static XInput instance{};
		return &instance;
	}
	/// @brief 更新する
	void Update();

	/// @brief どれか一つでも押していたら有効
	/// @return 押した場合 true
	bool IsAnyPress() const
	{
		return gamePad_[0].stickL_.Length() > 0.1f ||
			   gamePad_[0].stickR_.Length() > 0.1f ||
			   gamePad_[0].button_ != false ||
			   gamePad_[0].triggerL_ ||
			   gamePad_[0].triggerR_;
	}

	/// @brief 指定したキーを押しているか
	/// @param[in] keyCode 押されたキー
	/// @return 押した場合 true
	bool IsPress(const KeyCode keyCode) const
	{
		return gamePad_[0].button_ & (WORD)keyCode;
	}
	/// @brief 指定したキーを押した瞬間か
	/// @param[in] keyCode 押されたキー
	/// @return 押した場合 true
	bool IsTrigger(const KeyCode keyCode) const
	{
		return (gamePad_[0].button_ & (WORD)keyCode) && !(gamePad_[1].button_ & (WORD)keyCode);
	}
	/// @brief 指定したキーを離しているか
	/// @param[in] keyCode 離したキー
	/// @return 離した場合 true
	bool IsRelease(const KeyCode keyCode) const
	{
		return !(gamePad_[0].button_ & (WORD)keyCode) && (gamePad_[1].button_ & (WORD)keyCode);
	}

	/// @brief 現在の状態を返す
	/// @return 現在の状態
	const VirtualPad *const GetState() const
	{
		return &gamePad_[0];
	}

private:
};

/// @class Input
/// @brief 入力全体の管理
class Input
{
	Input() = default;
	Input(const Input &) = delete;
	Input operator=(const Input &) = delete;
	~Input() = default;

public:
	/// @enum InputType
	/// @brief 入力の種類
	enum class InputType : uint32_t
	{
		kPad,	   // ゲームパッド
		kKeyBoard, // キーボード
	};

	static Input *const GetInstance()
	{
		static Input instance{};
		return &instance;
	}
	/// @brief 初期化
	/// @details 入力の取得先などを生成する｡\
	/// directInputとxInputを生成する｡
	void Init();
	/// @brief 更新する
	/// @details 入力を更新する｡最後の入力タイプも更新する｡
	void Update()
	{
		directInput_->Update();
		xInput_->Update();

		if (xInput_->IsAnyPress()) {
			inputType_ = InputType::kPad;
		} else if (directInput_->IsAnyPress()) {
			inputType_ = InputType::kKeyBoard;
		}
	}

	/// @brief DirectInputを返す
	/// @return DirectInput
	/// @details DirectInputのインスタンスを返す｡
	const DirectInput *const GetDirectInput() const
	{
		return directInput_;
	}
	/// @brief XInputを返す
	/// @return XInput
	/// @details XInputのインスタンスを返す｡
	const XInput *const GetXInput() const
	{
		return xInput_;
	}

	/// @brief 入力の種類を返す｡
	/// @return 入力の種類
	/// @details 最後に更新した入力の種類を返す｡
	InputType GetInputType() const
	{
		return inputType_;
	}

private:
	InputType inputType_{};

	DirectInput *directInput_ = nullptr;
	XInput *xInput_ = nullptr;
};
