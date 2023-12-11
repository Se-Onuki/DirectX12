#pragma once
#include <xaudio2.h>

#pragma comment(lib,"xaudio2.lib")
#include <fstream>

#include <wrl.h>

#include <memory>
#include <array>
#include <string>
#include <set>

class Audio {
	Audio() = default;
	Audio(const Audio &) = delete;
	Audio operator=(const Audio &) = delete;
	~Audio() = default;

public:
	/// @brief チャンクヘッダ
	struct ChunkHeader {
		char id[4];		// chunk用のID
		int32_t size;	// チャンクサイズ
	};

	/// @brief RIFFヘッダチャンク
	struct RiffHeader {
		ChunkHeader chunk;	// "RIFF"
		char type[4];		// "WAVE"
	};

	/// @brief FMTチャンク
	struct FormatChunk {
		ChunkHeader chunk;	// "fmt"
		WAVEFORMATEX fmt;	// 波形フォーマット
	};

	// 音声データ
	struct SoundData {
		// 波形フォーマット
		WAVEFORMATEX wfex;
		// バッファの先頭アドレス
		BYTE *pBuffer;
		// バッファのサイズ
		uint32_t bufferSize;

		void Unload();
	};

	// 再生データ
	struct Voice {
		uint32_t handle = 0u;
		IXAudio2SourceVoice *sourceVoice = nullptr;
	};

	/// <summary>
/// オーディオコールバック
/// </summary>
	class XAudio2VoiceCallback : public IXAudio2VoiceCallback {
	public:
		// ボイス処理パスの開始時
		STDMETHOD_(void, OnVoiceProcessingPassStart)(THIS_ UINT32) {};
		// ボイス処理パスの終了時
		STDMETHOD_(void, OnVoiceProcessingPassEnd)(THIS) {};
		// バッファストリームの再生が終了した時
		STDMETHOD_(void, OnStreamEnd)(THIS) {};
		// バッファの使用開始時
		STDMETHOD_(void, OnBufferStart)(THIS_ void *) {};
		// バッファの末尾に達した時
		STDMETHOD_(void, OnBufferEnd)(THIS_ void *pBufferContext);
		// 再生がループ位置に達した時
		STDMETHOD_(void, OnLoopEnd)(THIS_ void *) {};
		// ボイスの実行エラー時
		STDMETHOD_(void, OnVoiceError)(THIS_ void *, HRESULT) {};
	};


	static Audio *const GetInstance() {
		static Audio instatnce{};
		return &instatnce;
	}

	void Finalize();

	void StaticInit();
	void PlayWave(const SoundData &soundData, bool loopFlag, float volume);
	uint32_t PlayWave(uint32_t index, bool loopFlag, float volume);

	bool IsPlaying(uint32_t voiceHandle);

	void StopWave(uint32_t voiceHandle);
	void StopAllWave();
	uint32_t LoadWave(const char *filename);
	SoundData *const GetWave(const uint32_t index);
private:

	Microsoft::WRL::ComPtr<IXAudio2> xAudio2_ = nullptr;
	IXAudio2MasteringVoice *masterVoice_ = nullptr;

	std::array<std::unique_ptr<SoundData>, 512u> soundArray_;
	std::set<Voice *> voices_;
	// 次に使う再生中データの番号
	uint32_t indexVoice_ = 0u;
	// オーディオコールバック
	XAudio2VoiceCallback voiceCallback_;

};

Audio::SoundData SoundLoadWave(const char *filename);