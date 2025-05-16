#pragma once
#include <cstdint>
#include <bit>
#include <span>
#include <array>
#include <memory>

#include "../HID/HidDeviceHolder.h"
#include <deque>

namespace SoLib {

	// Joy-Con関係のID
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

	/// @brief Joy-Conのデータのバイナリ保存
	class BinaryJoyConData {
	public:

		enum class JoyMemoryOffset : uint32_t {
			kReportID_ = 0u,		// レポートID(コントローラの状態)
			kTimer_ = 1u,			// 遅延用のタイマー
			kBattery_Connect_ = 2u,	// 上:バッテリー(0~8) / 下:接続状態(x0:プロコン, x1:Switch接続, xE:Joy-Con)
			kRightButton_ = 3u,		// 右ボタン
			kShareButton_ = 4u,		// 共有ボタン
			kLeftButton_ = 5u,		// 左ボタン
			kRightStick = 6u,		// 右スティック([6]~[8])
			kLeftStick = 9u,		// 左スティック([9]~[11])
			kVibratorReport_ = 12u,	// 振動を行った状態とかの情報
			kTransform_ = 13u,		// 6軸データを3回分
			kEnd = 49u				// 終わり (レポートIDがx31ならNFCなどの情報)
		};

		enum class JoySensorOffset : uint32_t {
			kAccel_X = 0u,
			kAccel_Y = 2u,
			kAccel_Z = 4u,
			kGyro_1 = 6u,
			kGyro_2 = 8u,
			kGyro_3 = 10,

			// https://github.com/dekuNukem/Nintendo_Switch_Reverse_Engineering/blob/master/imu_sensor_notes.md
		};

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

		/// @brief データからメモリを取得する
		/// @param[in] offset 参照先
		/// @return 参照先のデータ
		const std::byte *GetJoyMemory(JoyMemoryOffset offset) const { return &data_[static_cast<uint32_t>(offset)]; }

		/// @brief センサーの取得
		/// @return センサーの情報群
		std::span<const std::byte> GetSensor() const { return { &data_[static_cast<uint32_t>(JoyMemoryOffset::kTransform_)], 12u * 3u }; }

		std::array<std::byte, static_cast<size_t>(JoyMemoryOffset::kEnd)> data_{};
		uint8_t prevTime_ = 0;
		int32_t response_ = 0;

		uint32_t GetTimeSpan() const;

		/// @brief hidから取得したデータを格納する
		/// @param[in] device デバイスのデータ
		/// @return 読み取ったデータ量
		int32_t ReadInputReport(hid_device_ *device);

	private:
	};

	class JoyConUpdater
	{
	public:
		JoyConUpdater() = default;
		~JoyConUpdater() = default;

		/// @brief Joy-Conのデバイス登録
		/// @param[in] right 右Joy-Con
		/// @param[in] left 左Joy-Con
		void Init(hid_device_ *right, hid_device_ *left);

		/// @brief 更新処理
		void Update();

		operator bool() const { return joyConL_ and joyConR_ and isUpdate_; }

		/// @brief Joy-Conとデバイスのペア
		using JoyconDevicePair = std::pair<hid_device_ *, BinaryJoyConData>;

		/// @brief Joy-Conのデータを返す
		/// @return Joy-Conのデータ
		const std::array<const BinaryJoyConData, 2u> GetJoyConRL() const;

	private:

		/// @brief デバイスからデータを読み取る
		void GetInputData();

		std::unique_ptr<JoyconDevicePair> joyConR_;
		std::unique_ptr<JoyconDevicePair> joyConL_;

	};

}