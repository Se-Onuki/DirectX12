#pragma once
#include <wrl.h>
#include <cstdint>

class TextureManager
{
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	TextureManager() = default;
	TextureManager(const TextureManager &) = delete;
	TextureManager operator=(const TextureManager &) = delete;
	~TextureManager() = default;

	static const uint32_t maxTextureCount = 256u;

public:

	struct Texture{
		
	};

	static inline TextureManager *const GetInstance() {
		static TextureManager instance{};
		return &instance;
	}


private:

};
