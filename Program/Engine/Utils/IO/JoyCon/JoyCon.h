#pragma once
#include <functional>
#include <string>
#include <span>
#include <array>
#include <memory>
#include <bit>

#include "hidapi.h"
#include "../HID/HidDeviceHolder.h"
#include <bitset>

namespace SoLib {

	namespace JOYCON_IDs {
		// 任天堂のID
		inline static constexpr uint32_t NINTENDO_VENDOR_ID = 0x57E;
		// 左 Joy-ConのID
		inline static constexpr uint32_t JOYCON_L_PRODUCT_ID = 0x2006;
		// 右 Joy-ConのID
		inline static constexpr uint32_t JOYCON_R_PRODUCT_ID = 0x2007;
	}

	/// @brief Joy-Conのデータのバイナリ保存
	class BinaryJoyConData;

	/// @brief HidApi経由でJoy-Conデバイスにコマンドを送る
	/// @param[in] device デバイス
	/// @param[in] subcommandId コマンドのID
	/// @param[in] args コマンドのパラメータ
	/// @return 書き込まれた量､もしくはエラーの-1を返す
	bool SendSubcommand(hid_device *device, std::byte subcommandId, const std::span<std::byte> &args);

	/// @brief デバイスからデータを取得する
	/// @param[in] device デバイス
	/// @param[out] memory 書き込み先
	void ReadInputReport(hid_device *device, BinaryJoyConData &memory);

	/// @brief Joy-Conを探すための関数
	/// @param[in] deviceInfo デバイスの情報
	/// @return Joy-Conなら対応した名前､違うなら空文字
	std::string FuncFindJoyCon(const HidDeviceInfo deviceInfo);

	class BinaryJoyConData {
	public:
		enum class JoyButtonR : uint8_t {
			bY_ = 0x1,
			bX_ = 0x2,
			bB_ = 0x4,
			bA_ = 0x08,
			bSR_ = 0x10,
			bSL_ = 0x20,
			bR_ = 0x40,
			bZR_ = 0x80,
		};
		enum class JoyButtonL : uint8_t {
			bDown_ = 0x1,
			bUp_ = 0x2,
			bRight_ = 0x4,
			bLeft_ = 0x08,
			bSR_ = 0x10,
			bSL_ = 0x20,
			bL_ = 0x40,
			bZL_ = 0x80,
		};
		enum class JoyButtonShared : uint8_t {
			bMinus_ = 0x1,
			bPlus_ = 0x2,
			bStickR_ = 0x4,
			bStickL_ = 0x08,
			bHome_ = 0x10,
			bCapture_ = 0x20,

			bChargeingGrip_ = 0x80,
		};

		auto begin() { return data_.begin(); }
		auto begin() const { return data_.begin(); }
		auto cbegin() const { return data_.cbegin(); }

		auto end() { return data_.begin() + response_; }
		auto end() const { return data_.begin() + response_; }
		auto cend() const { return data_.cbegin() + response_; }

		const std::byte &operator[](const size_t i) const { return data_[i]; }

		auto *data() { return data_.data(); }

		const size_t size() const { return response_; }

		/// @brief レスポンス(応答)が無い場合は無効
		explicit operator bool() const { return response_; }

		std::array<std::byte, 0x40> data_{};
		int32_t response_ = 0;

		/// @brief hidから取得したデータを格納する
		/// @param[in] device デバイスのデータ
		/// @return 読み取ったデータ量
		int32_t ReadInputReport(hid_device_ *device);

	private:
	};


	class JoyConR
	{
	public:
		JoyConR() = default;
		~JoyConR() = default;

		bool IsPress(BinaryJoyConData::JoyButtonR) const;

		//private:

		uint16_t button_;

	};

	class JoyConL
	{
	public:
		JoyConL() = default;
		~JoyConL() = default;

		bool IsPress(BinaryJoyConData::JoyButtonL) const;

	private:

		uint16_t button_;

	};

	class JoyConPair {
	public:


	private:

	};



	class JoyConUpdater
	{
	public:
		JoyConUpdater() = default;
		~JoyConUpdater() = default;

		void Init(hid_device_ *right, hid_device_ *left);

		void Update();

		operator bool() const { return joyConL_ and joyConR_ and isUpdate_; }

		using JoyconDevicePair = std::pair<hid_device_ *, BinaryJoyConData>;

		const std::array<const BinaryJoyConData, 2u> GetJoyConRL() const;

		/// @brief 更新の停止
		void StopUpdate();

	private:

		void GetInputData();

		std::unique_ptr<JoyconDevicePair> joyConR_;
		std::unique_ptr<JoyconDevicePair> joyConL_;

		bool isUpdate_ = true;

	};



}