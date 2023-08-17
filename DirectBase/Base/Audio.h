#pragma once
#include <xaudio2.h>

#pragma comment(lib,"xaudio2.lib")
#include <fstream>

#include <wrl.h>

#include <memory>
#include <array>
#include <string>

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


	static Audio *const GetInstance() {
		static Audio instatnce{};
		return &instatnce;
	}

	void Finalize();

	void StaticInit();
	void PlayWave(const SoundData &soundData);
	void PlayWave(uint32_t index);
	uint32_t LoadWave(const char *filename);
	SoundData *const GetWave(const uint32_t index);
private:

	Microsoft::WRL::ComPtr<IXAudio2> xAudio2_ = nullptr;
	IXAudio2MasteringVoice *masterVoice_ = nullptr;

	std::array<std::unique_ptr<SoundData>, 128u> soundArray_;
};

Audio::SoundData SoundLoadWave(const char *filename);