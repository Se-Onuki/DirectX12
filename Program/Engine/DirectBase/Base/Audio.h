/// @file Audio.h
/// @brief 音を再生するクラス
/// @author ONUKI seiya
#pragma once
#include <xaudio2.h>

#pragma comment(lib, "xaudio2.lib")

#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>

#pragma comment(lib, "Mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "Mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")

#include <fstream>

#include <wrl.h>

#include "MemoryUsageManager.h"
#include <array>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>

/// @class Audio
/// @brief 音のマネージャクラス
class Audio
{
	Audio() = default;
	Audio(const Audio &) = delete;
	Audio operator=(const Audio &) = delete;
	~Audio() = default;

public:
	/// @struct ChunkHeader
	/// @brief チャンクヘッダ
	struct ChunkHeader
	{
		int32_t size;			// チャンクサイズ
		std::array<char, 4> id; // chunk用のID
	};

	/// @struct RiffHeader
	/// @brief RIFFヘッダチャンク
	struct RiffHeader
	{
		ChunkHeader chunk;		  // "RIFF"
		std::array<char, 4> type; // "WAVE"
	};

	/// @struct FormatChunk
	/// @brief FMTチャンク
	struct FormatChunk
	{
		ChunkHeader chunk; // "fmt"
		WAVEFORMATEX fmt;  // 波形フォーマット
	};

	//// @struct SoundData
	/// @brief 音声データ
	struct SoundData
	{
		// バッファの先頭アドレス
		std::unique_ptr<std::byte[]> pBuffer;
		// バッファのサイズ
		uint32_t bufferSize;
		// 波形フォーマット
		WAVEFORMATEX wfex;

        /// @fn void Unload(void)
		/// @brief 音声データの破棄
		void Unload();
	};

    /// @struct Voice
	/// @brief 再生データ
	struct Voice
	{
		bool operator==(Voice other) const
		{
			return this->sourceVoice == other.sourceVoice;
		}

		IXAudio2SourceVoice *sourceVoice = nullptr;
	};

    /// @struct VoiceHash
	/// @brief 再生データのハッシュ計算
	struct VoiceHash
	{
		/// @brief ハッシュ計算
		size_t operator()(const Voice &voice) const
		{
			return std::hash<IXAudio2SourceVoice *>()(voice.sourceVoice);
		}
	};

    /// @struct SoundHandle
	/// @brief 音のハンドル
	struct SoundHandle
	{

		SoundHandle() = default;
		SoundHandle(const SoundHandle &) = default;
		SoundHandle(SoundHandle &&) = default;
		SoundHandle &operator=(const SoundHandle &) = default;
		SoundHandle &operator=(SoundHandle &&) = default;

		SoundHandle(const uint32_t handle) : handle_(handle) {};
		SoundHandle &operator=(const uint32_t handle)
		{
			handle_ = handle;
			return *this;
		}

		/// @brief ハンドルをuint32_tへキャスト
		inline operator uint32_t() const { return handle_; }

        /// @fn uint32_t Get(void)
		/// @brief ハンドルを取得
		/// @return ハンドル
		uint32_t Get() const { return handle_; }
		/// @fn Voice Play(bool, float)
		/// @brief 音の再生
		/// @param[in] loopFlag ループフラグ
		/// @param[in] volume 音量
		/// @return 再生したボイス
		Voice Play(bool loopFlag, float volume) const { return audio_->PlayWave(handle_, loopFlag, volume); }

	private:
		/// @brief 音のハンドル
		uint32_t handle_ = (std::numeric_limits<uint32_t>::max)();
		/// @brief オーディオマネージャのアドレス
		static Audio *const audio_;
	};

    /// @struct VoiceHandle
	/// @brief ボイスのハンドル
	struct VoiceHandle
	{
		/// @brief ボイスハンドルのタイプ
		using HandleType = Voice *;

		VoiceHandle() = default;
		VoiceHandle(const HandleType handle) : handle_(handle) {}
		VoiceHandle &operator=(const HandleType handle)
		{
			handle_ = handle;
			return *this;
		}

		inline operator HandleType() const { return handle_; }
        /// @fn HandleType Get(void)
		/// @brief ボイスのハンドルを取得
        /// @return ハンドル
		HandleType Get() const { return handle_; }

	private:
		/// @brief ボイスハンドル
		HandleType handle_ = nullptr;
	};

    /// @class XAudio2VoiceCallback
	/// @brief オーディオコールバック
	class XAudio2VoiceCallback : public IXAudio2VoiceCallback
	{
		Audio *pAudio_ = nullptr;

	public:
		XAudio2VoiceCallback() = default;
		XAudio2VoiceCallback(Audio *audio) : pAudio_(audio) {}
		// ボイス処理パスの開始時
		STDMETHOD_(void, OnVoiceProcessingPassStart)
		(UINT32){};
		// ボイス処理パスの終了時
		STDMETHOD_(void, OnVoiceProcessingPassEnd)
		(){};
		// バッファストリームの再生が終了した時
		STDMETHOD_(void, OnStreamEnd)
		(){};
		// バッファの使用開始時
		STDMETHOD_(void, OnBufferStart)
		(void *){};
		// バッファの末尾に達した時
		STDMETHOD_(void, OnBufferEnd)
		(void *pBufferContext);
		// 再生がループ位置に達した時
		STDMETHOD_(void, OnLoopEnd)
		(void *){};
		// ボイスの実行エラー時
		STDMETHOD_(void, OnVoiceError)
		(void *, HRESULT){};
	};

	/// @fn Audio*const GetInstance(void)
	/// @brief インスタンスを取得する
	/// @return インスタンスのアドレス
	static Audio *const GetInstance()
	{
		static Audio instatnce{};
		return &instatnce;
	}

	/// @fn void Finlize(void)
	/// @brief インスタンスを破棄する
	void Finalize();

	/// @fn void StaticInit(void)
	/// @brief 静的初期化を行う
	void StaticInit();
	/// @fn Voice PlayWave(const SoundData &, bool, float)
	/// @brief 音データを再生する
	/// @param[in] soundData 再生する音データ
	/// @param[in] loopFlag ループフラグ
	/// @param[in] volume 音量
	/// @return 再生したボイス
	Voice PlayWave(const SoundData &soundData, bool loopFlag, float volume);
	/// @fn Voice PlayWave(uint32_t, bool, float)
	/// @brief 音データのインデックスから再生する
	/// @param[in] index 再生する音データのインデックス
	/// @param[in] loopFlag ループフラグ
	/// @param[in] volume 音量
	/// @return 再生したボイス
	Voice PlayWave(uint32_t index, bool loopFlag, float volume);

	/// @fn bool IsPlaying(VoiceHandle) const
	/// @brief ボイスが再生中か
	/// @param[in] voiceHandle ボイスハンドル
	/// @return 再生中:True, 停止中:False
	bool IsPlaying(Voice voiceHandle) const;

	/// @fn void SetVolume(Voice &, float)
	/// @brief 音量を設定する
	/// param[in] voice 設定したい音声のボイスハンドル
	/// param[in] volume 音量
	void SetVolume(Voice &voice, const float volume);

	/// @fn void StopWave(Voice &)
	/// @brief 音声を停止する
	/// @param[in] voiceHandle 停止したい音声のボイスハンドル
	void StopWave(Voice &voiceHandle);
	/// @fn void StopAllWave(void)
	/// @brief 全ての音声を停止する
	void StopAllWave();
	/// @fn uint32_t LoadWave(const char *)
	/// @brief ファイルからWave音声を読み込む
	/// @param[in] filename 読み込むファイル名
	/// @return 読み込んだ音声のインデックス
	uint32_t LoadWave(const char *filename);
	/// @fn uint32_t LoadMP3(const char *)
	/// @brief ファイルから音声を読み込む
	/// @param[in] filename 読み込むファイル名
	/// @return 読み込んだ音声のインデックス
	uint32_t LoadMP3(const char *filename);
	/// @fn GetWave(const uint32_t)
	/// @brief インデックスから音声データを得る
	/// @param[in] index 読み込んた音声のインデックス
	/// @return 読み込んだ音声データ
	SoundData *const GetWave(const uint32_t index);

	/// @fn void ImGuiWidget(void)
	/// @brief ImGui画面に表示するウィジェットを追加する
	/// @return 何かを変更したらTrue
	bool ImGuiWidget();

public:
	/// @brief 音声の最大数
	static const uint32_t kMaxSound = 512u;

private:
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2_ = nullptr;
	IXAudio2MasteringVoice *masterVoice_ = nullptr;

	std::array<std::unique_ptr<SoundData>, kMaxSound> soundArray_;
	std::unordered_map<std::string, uint32_t> fileMap_;
	std::unordered_set<Voice, VoiceHash> voices_;

	// オーディオコールバック
	XAudio2VoiceCallback voiceCallback_;
};

/// @fn SoundLoadWave(count char*)
/// @brief Wave音声を読み込む
/// @param[in] filename 読み込むファイル名
/// @return 読み込んた音声データ
Audio::SoundData SoundLoadWave(const char *filename);

/// @fn SoundLoadMP3(count char*)
/// @brief 音声を読み込む
/// @param[in] filename 読み込むファイル名
/// @return 読み込んた音声データ
Audio::SoundData SoundLoadMP3(const char *filename);