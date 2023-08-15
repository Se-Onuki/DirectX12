#pragma once
#include <xaudio2.h>

#pragma comment(lib,"xaudio2.lib")
#include <fstream>

#include <wrl.h>

class Audio {
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2_ = nullptr;
	IXAudio2MasteringVoice *masterVoice_ = nullptr;

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


	static Audio *const GetInstance() {
		static Audio instatnce{};
		return &instatnce;
	}

	void StaticInit();

private:
};

Audio::SoundData SoundLoadWave(const char *const filename);