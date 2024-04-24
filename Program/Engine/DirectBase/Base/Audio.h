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

class Audio {
	Audio() = default;
	Audio(const Audio &) = delete;
	Audio operator=(const Audio &) = delete;
	~Audio() = default;

public:
	/// @brief チャンクヘッダ
	struct ChunkHeader {
		char id[4];   // chunk用のID
		int32_t size; // チャンクサイズ
	};

	/// @brief RIFFヘッダチャンク
	struct RiffHeader {
		ChunkHeader chunk; // "RIFF"
		char type[4];      // "WAVE"
	};

	/// @brief FMTチャンク
	struct FormatChunk {
		ChunkHeader chunk; // "fmt"
		WAVEFORMATEX fmt;  // 波形フォーマット
	};

	// 音声データ
	struct SoundData {
		// 波形フォーマット
		WAVEFORMATEX wfex;
		// バッファの先頭アドレス
		std::unique_ptr<BYTE[]> pBuffer;
		// バッファのサイズ
		uint32_t bufferSize;

		void Unload();
	};

	// 再生データ
	struct Voice {
		bool operator==(Voice other) const
		{
			return this->sourceVoice == other.sourceVoice;
		}

		IXAudio2SourceVoice *sourceVoice = nullptr;
	};

	struct VoiceHash {
		size_t operator()(const Voice &voice) const
		{
			return std::hash<IXAudio2SourceVoice *>()(voice.sourceVoice);
		}
	};

	struct SoundHandle {

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

		inline operator uint32_t() const { return handle_; }

		uint32_t Get() const { return handle_; }

		Voice Play(bool loopFlag, float volume) const { return audio_->PlayWave(handle_, loopFlag, volume); }

	private:
		uint32_t handle_ = (std::numeric_limits<uint32_t>::max)();
		static Audio *const audio_;
	};

	struct VoiceHandle {
		using HandleType = Voice *;

		VoiceHandle() = default;
		VoiceHandle(const HandleType handle) : handle_(handle) {}
		VoiceHandle &operator=(const HandleType handle)
		{
			handle_ = handle;
			return *this;
		}

		inline operator HandleType() const { return handle_; }

		HandleType Get() const { return handle_; }

	private:
		HandleType handle_ = nullptr;
	};

	/// <summary>
	/// オーディオコールバック
	/// </summary>
	class XAudio2VoiceCallback : public IXAudio2VoiceCallback {
	public:
		// ボイス処理パスの開始時
		STDMETHOD_(void, OnVoiceProcessingPassStart)
			(UINT32) {};
		// ボイス処理パスの終了時
		STDMETHOD_(void, OnVoiceProcessingPassEnd)
			() {};
		// バッファストリームの再生が終了した時
		STDMETHOD_(void, OnStreamEnd)
			() {};
		// バッファの使用開始時
		STDMETHOD_(void, OnBufferStart)
			(void *) {};
		// バッファの末尾に達した時
		STDMETHOD_(void, OnBufferEnd)
			(void *pBufferContext);
		// 再生がループ位置に達した時
		STDMETHOD_(void, OnLoopEnd)
			(void *) {};
		// ボイスの実行エラー時
		STDMETHOD_(void, OnVoiceError)
			(void *, HRESULT) {};
	};

	static Audio *const GetInstance()
	{
		static Audio instatnce{};
		return &instatnce;
	}

	void Finalize();

	void StaticInit();
	Voice PlayWave(const SoundData &soundData, bool loopFlag, float volume);
	Voice PlayWave(uint32_t index, bool loopFlag, float volume);

	bool IsPlaying(Voice voiceHandle);

	void StopWave(Voice &voiceHandle);
	void StopAllWave();
	uint32_t LoadWave(const char *filename);

	uint32_t LoadMP3(const char *filename);
	SoundData *const GetWave(const uint32_t index);

	bool ImGuiWidget();

public:
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

Audio::SoundData SoundLoadWave(const char *filename);

Audio::SoundData SoundLoadMP3(const char *filename);