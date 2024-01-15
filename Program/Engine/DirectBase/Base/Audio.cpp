#include "Audio.h"
#include <cassert>

void Audio::Finalize() {
	xAudio2_.Reset();
	for (auto &sound : soundArray_) {
		if (sound) {
			sound->Unload();
		}
	}
}

void Audio::StaticInit() {

	HRESULT hr = S_FALSE;

	hr = XAudio2Create(&xAudio2_, 0u, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(hr));

	hr = xAudio2_->CreateMasteringVoice(&masterVoice_);
	assert(SUCCEEDED(hr));

	fileMap_.clear();
	voices_.clear();

}

Audio::Voice Audio::PlayWave(const SoundData &soundData, bool loopFlag, float volume) {
	HRESULT hr = S_FALSE;


	//	auto handle = indexManager_.RequestRange(1u);

		// 波形フォーマットを基にSourceVoiceの生成
	IXAudio2SourceVoice *pSourceVoice = nullptr;
	hr = xAudio2_->CreateSourceVoice(&pSourceVoice, &soundData.wfex, 0, 2.0f, &voiceCallback_);
	assert(SUCCEEDED(hr));

	// 再生中データ
	auto voice = Voice{};
	voice.sourceVoice = pSourceVoice;
	// 再生中データコンテナに登録
	voices_.insert(voice);

	// 再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundData.pBuffer;
	buf.AudioBytes = soundData.bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	if (loopFlag) {
		// 無限ループ
		buf.LoopCount = XAUDIO2_LOOP_INFINITE;
	}

	// 波形データの再生
	hr = pSourceVoice->SubmitSourceBuffer(&buf);
	pSourceVoice->SetVolume(volume);
	hr = pSourceVoice->Start();


	return voice;
}

Audio::Voice Audio::PlayWave(uint32_t index, bool loopFlag, float volume) {
	return PlayWave(*GetWave(index), loopFlag, volume);
}

bool Audio::IsPlaying(Voice voiceHandle) {
	// 再生中リストから検索
	auto it = voices_.find(voiceHandle);
	// 発見。再生終わってるのかどうかを判断
	if (it != voices_.end()) {
		XAUDIO2_VOICE_STATE state{};
		it->sourceVoice->GetState(&state);
		return state.SamplesPlayed != 0;
	}
	return false;
}

void Audio::StopWave(Voice &voiceHandle) {
	// 再生中リストから検索
	auto it = voices_.find(voiceHandle);
	// 発見
	if (it != voices_.end()) {
		it->sourceVoice->DestroyVoice();

		voices_.erase(it);
		voiceHandle.sourceVoice = nullptr;
	}
}

void Audio::StopAllWave() {
	for (auto voice : voices_) {
		voice.sourceVoice->DestroyVoice();
	}
	voices_.clear();
}

uint32_t Audio::LoadWave(const char *filename) {

	// ファイルが読み込まれているか検知
	auto soundItr = fileMap_.find(filename);
	// もしファイルが読み込まれていたらそれを返す
	if (soundItr != fileMap_.end()) {
		return soundItr->second;
	}

	// 読み込まれていなかったら新しく読み込む
	for (uint32_t i = 0u; i < soundArray_.size(); i++) {
		auto &pSound = soundArray_[i];
		if (not pSound) {
			pSound = std::make_unique<SoundData>(SoundLoadWave(filename)); 	// ファイル読み込み
			fileMap_[filename] = i;	// ファイルを読み込んだときのインデックスを記録
			return i;	// インデックスを返す
		}
	}
	assert(0 && "Soundの追加に失敗しました");
	return (std::numeric_limits<uint32_t>::max)();
}

Audio::SoundData *const Audio::GetWave(uint32_t index) {
	if (index == (std::numeric_limits<uint32_t>::max)()) { return nullptr; }
	return soundArray_[index].get();
}
//
//uint32_t Audio::FindUnusedIndex() const {
//	return indexManager_.FindUnusedRange(1u);
//}

Audio::SoundData SoundLoadWave(const char *filename) {

	// HRESULT hr = S_FALSE;

#pragma region ファイルオープン

	// ファイル入力ストリームのインスタンス
	std::ifstream file;
	// .wavファイルをバイナリモードで開く
	file.open(filename, std::ios_base::binary);
	// ファイルオープン失敗の検出
	assert(file.is_open());

#pragma endregion

#pragma region .wavデータ読み込み

	// RIFFヘッダの読み込み
	Audio::RiffHeader riff;
	file.read((char *)&riff, sizeof(riff));

	// ファイルがRIFFかどうかチェック
	if (strncmp(riff.chunk.id, "RIFF", 4u) != 0) {
		assert(0 && "ファイルがRIFFではありません");
	}

	// タイプがWAVEかどうかチェック
	if (strncmp(riff.type, "WAVE", 4u) != 0) {
		assert(0 && "ファイルがWAVEではありません");
	}

	// Formatチャンクの読み込み
	Audio::FormatChunk format{};
	// チャンクヘッダの確認
	file.read((char *)&format, sizeof(Audio::ChunkHeader));
	// Junkチャンクを検出した場合
	if (strncmp(format.chunk.id, "JUNK", 4u) == 0) {
		// 読み取り範囲をJunkチャンクの終わりまで進める
		file.seekg(format.chunk.size, std::ios_base::cur);
		file.read((char *)&format, sizeof(Audio::ChunkHeader));
	}
	if (strncmp(format.chunk.id, "fmt ", 4u) != 0) {
		assert(0);
	}

	// チャンク本体の読み込み
	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char *)&format.fmt, format.chunk.size);

	// Dataチャンク読み込み	
	Audio::ChunkHeader data{};
	file.read((char *)&data, sizeof(data));
	// Junkチャンクを検出した場合
	if (strncmp(data.id, "JUNK", 4u) == 0) {
		// 読み取り範囲をJunkチャンクの終わりまで進める
		file.seekg(data.size, std::ios_base::cur);
		// 再読み込み
		file.read((char *)&data, sizeof(data));
	}

	// 実際に読み込む
	if (strncmp(data.id, "data", 4u) != 0) {
		assert(0);
	}

	// Dataチャンクのデータ部(波形データ)の読み込み
	char *const pBuffer = new char[data.size];
	file.read(pBuffer, data.size);

	// waveファイルを閉じる
	file.close();

#pragma endregion

#pragma region 読み込んだ音声データをreturn

	// returnするための音声データ
	Audio::SoundData soundData{};

	soundData.wfex = format.fmt;
	soundData.pBuffer = reinterpret_cast<BYTE *const>(pBuffer);
	soundData.bufferSize = data.size;

#pragma endregion

	return soundData;
}

void Audio::SoundData::Unload()
{
	// バッファのメモリを解放
	if (pBuffer) delete[] pBuffer;

	pBuffer = nullptr;
	bufferSize = 0u;
	wfex = {};
}
//
//STDMETHODIMP_(void __stdcall) Audio::XAudio2VoiceCallback::OnBufferEnd(void* pBufferContext)
//{
//}
void Audio::XAudio2VoiceCallback::OnBufferEnd(THIS_ void *pBufferContext) {

	Voice *voice = reinterpret_cast<Voice *>(pBufferContext);
	// 再生リストから除外
	Audio::GetInstance()->voices_.erase(*voice);
}
