#include "DxResource.h"
#include "../Base/DirectXCommon.h"

namespace SolEngine {
	DxResource DxResource::Create(const uint32_t dataSize, const uint32_t dataCount)
	{
		ID3D12Device *const device = GetDevice();

		DxResource result;
		result.resource_ = CreateBufferResource(device, (static_cast<size_t>(dataSize)*dataCount + 0xffllu) & ~0xffllu);

		HRESULT hr = S_FALSE;

		hr = result.resource_->Map(0, nullptr, reinterpret_cast<void **>(&result.mapPtr_));

		assert(SUCCEEDED(hr));

		return result;
	}
}