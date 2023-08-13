#include "Audio.h"
#include <cassert>

void Audio::StaticInit() {

	HRESULT hr = S_FALSE;

	hr = XAudio2Create(&xAudio2_, 0u, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(hr));

	hr = xAudio2_->CreateMasteringVoice(&masterVoice_);
	assert(SUCCEEDED(hr));

}

Audio::SoundData SoundLoadWave(const char *const filename) {
	filename;
	return Audio::SoundData{};
}