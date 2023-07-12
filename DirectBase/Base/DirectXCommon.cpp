#include "DirectXCommon.h"


void DirectXCommon::Init(WinApp *winApp, int32_t backBufferWidth, int32_t backBufferHeight)
{
	assert(winApp);
	assert(4 <= backBufferWidth && backBufferWidth <= 4096);
	assert(4 <= backBufferHeight && backBufferHeight <= 4096);

	winApp_ = winApp;
	backBufferWidth_ = backBufferWidth;
	backBufferHeight_ = backBufferHeight;
}

DirectXCommon *const DirectXCommon::GetInstance()
{
	static DirectXCommon instance;
	return &instance;
}
