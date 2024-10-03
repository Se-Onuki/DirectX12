#include "Audio.h"
#include "../../Engine/Utils/SoLib/SoLib.h"
#include "../String/String.h"
#include <cassert>
#include <execution>

Audio *const Audio::SoundHandle::audio_ = Audio::GetInstance();

void Audio::Finalize()
{
	HRESULT hr = S_FALSE;
	xAudio2_.Reset();
	for (auto &sound : soundArray_) {
		if (sound) {
			sound->Unload();
		}
	}

	hr = MFShutdown();
	assert(SUCCEEDED(hr));

}

void Audio::StaticInit()
{

	HRESULT hr = S_FALSE;

	//// COMの初期化
	//hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	//assert(SUCCEEDED(hr));

	// Media Foundationの初期化
	hr = MFStartup(MF_VERSION, MFSTARTUP_NOSOCKET);
	assert(SUCCEEDED(hr));

	// xAudioの設定

	hr = XAudio2Create(&xAudio2_, 0u, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(hr));

	hr = xAudio2_->CreateMasteringVoice(&masterVoice_);
	assert(SUCCEEDED(hr));

	fileMap_.clear();
	voices_.clear();

	// オーディオコールバックの初期化
	voiceCallback_ = XAudio2VoiceCallback{ GetInstance() };
}

Audio::Voice Audio::PlayWave(const SoundData &soundData, bool loopFlag, float volume)
{
	HRESULT hr = S_FALSE;

	//	auto handle = indexManager_.RequestRange(1u);

	// 波形フォーマットを基にSourceVoiceの生成
	IXAudio2SourceVoice *pSourceVoice = nullptr;
	hr = xAudio2_->CreateSourceVoice(&pSourceVoice, &soundData.wfex, 0, 2.0f, &voiceCallback_);
	assert(SUCCEEDED(hr));

	// 再生中データ
	auto voice = Voice{ .sourceVoice = pSourceVoice };
	// 再生中データコンテナに登録
	voices_.insert(voice);

	// 再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = reinterpret_cast<const BYTE *>(soundData.pBuffer.get());
	buf.pContext = voice.sourceVoice;
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

Audio::Voice Audio::PlayWave(uint32_t index, bool loopFlag, float volume)
{
	return PlayWave(*GetWave(index), loopFlag, volume);
}

bool Audio::IsPlaying(Voice voiceHandle) const
{
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

void Audio::SetVolume(Voice &voice, const float volume)
{
	voice.sourceVoice->SetVolume(volume);
}

void Audio::StopWave(Voice &voiceHandle)
{
	// 再生中リストから検索
	auto it = voices_.find(voiceHandle);
	// 発見
	if (it != voices_.end()) {
		it->sourceVoice->DestroyVoice();

		voices_.erase(it);
	}
}

void Audio::StopAllWave()
{
	for (auto voice : voices_) {
		voice.sourceVoice->DestroyVoice();
	}
	voices_.clear();
}

uint32_t Audio::LoadWave(const char *filename)
{

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
			pSound = std::make_unique<SoundData>(SoundLoadWave(filename)); // ファイル読み込み
			fileMap_[filename] = i;                                        // ファイルを読み込んだときのインデックスを記録
			return i;                                                      // インデックスを返す
		}
	}
	assert(0 && "Soundの追加に失敗しました");
	return (std::numeric_limits<uint32_t>::max)();
}

uint32_t Audio::LoadMP3(const char *filename)
{
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
			pSound = std::make_unique<SoundData>(SoundLoadMP3(filename)); // ファイル読み込み
			fileMap_[filename] = i;                                        // ファイルを読み込んだときのインデックスを記録
			return i;                                                      // インデックスを返す
		}
	}
	assert(0 && "Soundの追加に失敗しました");
	return (std::numeric_limits<uint32_t>::max)();
}

Audio::SoundData *const Audio::GetWave(uint32_t index)
{
	if (index == (std::numeric_limits<uint32_t>::max)()) {
		return nullptr;
	}
	return soundArray_[index].get();
}
bool Audio::ImGuiWidget()
{

#ifdef USE_IMGUI

	ImGui::Begin("AudioManager");
	static auto itr = fileMap_.begin();

	itr = SoLib::ImGuiWidget("AudioList", &fileMap_, itr,
		[](const decltype(itr) &itemIndex) -> std::string
		{
			return '(' + std::to_string(itemIndex->second) + ')' + itemIndex->first;
		});

	if (ImGui::Button("Play")) {
		if (itr != fileMap_.end()) {
			PlayWave(itr->second, false, 1.f);
		}
	}

	ImGui::End();

#endif // USE_IMGUI

	return false;
}

//
// uint32_t Audio::FindUnusedIndex() const {
//	return indexManager_.FindUnusedRange(1u);
//}

Audio::SoundData SoundLoadWave(const char *filename)
{

	// HRESULT hr = S_FALSE;

#pragma region ファイルオープン

	// ファイル入力ストリームのインスタンス
	std::ifstream file;
	// .wavファイルをバイナリモードで開く
	file.open(filename, std::ios_base::binary);
	// ファイルオープン失敗の検出
	assert(file.is_open());

#pragma endregion

#pragma region.wavデータ読み込み

	// RIFFヘッダの読み込み
	Audio::RiffHeader riff;
	file.read((char *)&riff, sizeof(riff));

	// ファイルがRIFFかどうかチェック
	if (strncmp(riff.chunk.id.data(), "RIFF", 4u) != 0) {
		assert(0 && "ファイルがRIFFではありません");
	}

	// タイプがWAVEかどうかチェック
	if (strncmp(riff.type.data(), "WAVE", 4u) != 0) {
		assert(0 && "ファイルがWAVEではありません");
	}

	// Formatチャンクの読み込み
	Audio::FormatChunk format{};
	// チャンクヘッダの確認
	file.read((char *)&format, sizeof(Audio::ChunkHeader));
	// Junkチャンクを検出した場合
	if (strncmp(format.chunk.id.data(), "JUNK", 4u) == 0) {
		// 読み取り範囲をJunkチャンクの終わりまで進める
		file.seekg(format.chunk.size, std::ios_base::cur);
		file.read((char *)&format, sizeof(Audio::ChunkHeader));
	}
	if (strncmp(format.chunk.id.data(), "fmt ", 4u) != 0) {
		assert(0);
	}

	// チャンク本体の読み込み
	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char *)&format.fmt, format.chunk.size);

	// Dataチャンク読み込み
	Audio::ChunkHeader data{};
	file.read((char *)&data, sizeof(data));
	// Junkチャンクを検出した場合
	if (strncmp(data.id.data(), "JUNK", 4u) == 0) {
		// 読み取り範囲をJunkチャンクの終わりまで進める
		file.seekg(data.size, std::ios_base::cur);
		// 再読み込み
		file.read((char *)&data, sizeof(data));
	}

	// 実際に読み込む
	if (strncmp(data.id.data(), "data", 4u) != 0) {
		assert(0);
	}

	// Dataチャンクのデータ部(波形データ)の読み込み
	std::unique_ptr<std::byte[]> pBuffer = std::make_unique<std::byte[]>(data.size);
	file.read(reinterpret_cast<char *>(pBuffer.get()), data.size);

	// waveファイルを閉じる
	file.close();

#pragma endregion

#pragma region 読み込んだ音声データをreturn

	// returnするための音声データ
	Audio::SoundData soundData{};

	soundData.wfex = format.fmt;
	soundData.pBuffer = std::move(pBuffer);
	soundData.bufferSize = data.size;

#pragma endregion

	return soundData;
}

Audio::SoundData SoundLoadMP3(const char *filename)
{

	HRESULT hr = S_FALSE;

	// ソースリーダの作成
	IMFSourceReader *pSourceReader = nullptr;
	// ファイルパスを入れて読み込む
	MFCreateSourceReaderFromURL(ConvertString(filename).c_str(), nullptr, &pSourceReader);

	// メディアタイプの設定
	IMFMediaType *pMediaType = nullptr;
	hr = MFCreateMediaType(&pMediaType); // メディアタイプを作成して､ポインタを取得
	assert(SUCCEEDED(hr));
	// メディアタイプの設定
	hr = pMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
	assert(SUCCEEDED(hr));
	hr = pMediaType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
	assert(SUCCEEDED(hr));

	// SourceReaderにメディアタイプを設定
	hr = pSourceReader->SetCurrentMediaType(static_cast<DWORD>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), nullptr, pMediaType);
	assert(SUCCEEDED(hr));

	// メディアタイプのデータを破棄
	pMediaType->Release();
	pMediaType = nullptr; // 安全の確保

	// メディアタイプのデータの取得
	pSourceReader->GetCurrentMediaType(static_cast<DWORD>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), &pMediaType);

	// オーディオデータ形式の作成
	WAVEFORMATEX *waveFormat = nullptr;
	MFCreateWaveFormatExFromMFMediaType(pMediaType, &waveFormat, nullptr);

	// データの読み込み
	// ソースリーダから､サンプルを読み込んでvectorに格納していく
	std::vector<std::byte> mediaData;
	while (true) {
		// メディアのデータを取得するサンプラ
		IMFSample *pMFSample = nullptr;
		// 読み込みの状態を取得
		DWORD dwStreamFlag = 0;

		// ソースからサンプルを読み込む
		pSourceReader->ReadSample(static_cast<DWORD>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), 0, nullptr, &dwStreamFlag, nullptr, &pMFSample);

		// もし､読み込みが末尾に達したら終了
		if (dwStreamFlag & MF_SOURCE_READERF_ENDOFSTREAM) {
			break;
		}

		// メディアのバッファ
		IMFMediaBuffer *pMFMediaBuffer = nullptr;
		// バッファにデータを格納
		pMFSample->ConvertToContiguousBuffer(&pMFMediaBuffer);

		BYTE *pBuffer = nullptr;
		DWORD cbCurrentLength = 0;

		// メディアのバッファをロック
		pMFMediaBuffer->Lock(&pBuffer, nullptr, &cbCurrentLength);

		// メディアデータの長さを読み込んだデータを使って拡張する
		mediaData.resize(mediaData.size() + cbCurrentLength);

		// メディアデータを格納
		memcpy(mediaData.data() + mediaData.size() - cbCurrentLength, pBuffer, cbCurrentLength); // メディアデータの終端から､保存するデータの長さ分のデータを上書き

		// メディアデータのロックを解除する
		pMFMediaBuffer->Unlock();

		// メディアデータの開放
		pMFMediaBuffer->Release();
		pMFMediaBuffer = nullptr; // 安全確保
		// サンプラの開放
		pMFSample->Release();
		pMFSample = nullptr; // 安全確保

	}

	// xAudioとして作成する
	Audio::SoundData result;
	result.wfex = *waveFormat;	// データ型に置き換えて保存
	result.pBuffer = std::make_unique<std::byte[]>(mediaData.size());	// 保存領域を構築する
	result.bufferSize = static_cast<uint32_t>(mediaData.size());			// バッファのサイズを書き込む

	// データの移動
	std::move(std::execution::par_unseq, mediaData.cbegin(), mediaData.cend(), result.pBuffer.get());

	// メディアフォーマットのデータの破棄
	CoTaskMemFree(waveFormat);
	waveFormat = nullptr;

	pMediaType->Release();
	pMediaType = nullptr;

	pSourceReader->Release();
	pSourceReader = nullptr;

	return result;
}

void Audio::SoundData::Unload()
{
	// バッファのメモリを解放
	pBuffer.reset();
	bufferSize = 0u;
	wfex = {};
}

void Audio::XAudio2VoiceCallback::OnBufferEnd(void *pBufferContext)
{
	// ポインタが有効なら
	if (pBufferContext) {
		// 再生リストから除外
		pAudio_->voices_.erase(Voice{ reinterpret_cast<IXAudio2SourceVoice *>(pBufferContext) });
	}
}
