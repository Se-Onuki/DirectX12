#include "JoyConLoader.h"

namespace SoLib {

	std::string FuncFindJoyCon(const HidDeviceInfo info) {

		std::string result;

		// 任天堂のID以外は飛ばす
		if (info->vendor_id != JOYCON_IDs::NINTENDO_VENDOR_ID) {
			return result;
		}

		// デバイスのIDを取得する
		auto productId = info->product_id;

		// 対応したIDなら名前をつける
		switch (productId) {
		case JOYCON_IDs::JOYCON_L_PRODUCT_ID:
			result += "Joycon(L)";
			break;
		case JOYCON_IDs::JOYCON_R_PRODUCT_ID:
			result += "Joycon(R)";
			break;
		default:
			break;
		}

		return result;

	}

	bool SendSubcommand(hid_device *device, std::byte subcommandId, const std::span<std::byte> &args) {

		static uint8_t packetNumber = 0;
		std::array<std::byte, 0x40u> buffer{ std::byte(0) };
		buffer[0] = std::byte(0x01); // Output report ID（固定）

		buffer[1] = std::byte(packetNumber++);	// パケットカウント

		// 振動データ(8バイト)を全て0に（今回は無効化）
		std::fill(buffer.begin() + 2, buffer.begin() + 10, std::byte(0x00));

		buffer[10] = subcommandId;			// サブコマンドID
		std::copy(args.begin(), args.end(), buffer.begin() + 11); // 引数コピー

		return hid_write(device, std::bit_cast<const uint8_t *>(buffer.data()), buffer.size()) >= 0;
	}

	void ReadInputReport(hid_device *device, BinaryJoyConData &memory) {
		memory.response_ = hid_read_timeout(device, std::bit_cast<uint8_t *>(memory.data()), memory.data_.size(), 1);
	}

	int32_t BinaryJoyConData::ReadInputReport(hid_device_ *device)
	{
		SoLib::ReadInputReport(device, *this);
		return this->response_;
	}

	void JoyConUpdater::Init(hid_device_ *right, hid_device_ *left)
	{
		// 更新を行う設定にする
		isUpdate_ = true;

		// データの確保
		joyConR_ = std::make_unique<JoyconDevicePair>();
		joyConL_ = std::make_unique<JoyconDevicePair>();

		// データのコピー
		joyConR_->first = right;
		joyConL_->first = left;
	}

	void JoyConUpdater::Update()
	{
		//while (true) {
		//	// もしアップデートを続けられないなら
		//	if (not *this) {
		//		// 終わる
		//		return;
		//	}
		GetInputData();
		//}
	}

	const std::array<const BinaryJoyConData, 2u> JoyConUpdater::GetJoyConRL() const
	{
		return { joyConR_->second, joyConL_->second };
	}

	void JoyConUpdater::StopUpdate()
	{
		isUpdate_ = false;
	}

	void JoyConUpdater::GetInputData()
	{
		auto &joyL = joyConL_->second, &joyR = joyConR_->second;

		// 読み取ったデータを格納
		joyL.ReadInputReport(joyConL_->first);
		joyR.ReadInputReport(joyConR_->first);

		// 共用部分のデータの統合
		std::byte tmp = (joyL[4] | joyR[4]);
		// それぞれにデータを分配
		joyL.data_[4] = tmp;
		joyR.data_[4] = tmp;
	}
}