#include "SkyBox.h"
#include "../../Base/TextureManager.h"

namespace SolEngine {

	void SkyBox::SetTexture(const std::string &file_path)
	{
		textureHandle_ = ::TextureManager::Load(file_path);
	}
	void SkyBox::CreateBuffer()
	{
		std::array<ModelVertexData::VertexData, 4u * 2u> vertexData;

		vertexData[0].position = { +1.f, +1.f, +1.f, 1.f };
		vertexData[1].position = { +1.f, +1.f, -1.f, 1.f };
		vertexData[2].position = { +1.f, -1.f, +1.f, 1.f };
		vertexData[3].position = { +1.f, -1.f, -1.f, 1.f };

		vertexData[4].position = { -1.f, +1.f, -1.f, 1.f };
		vertexData[5].position = { -1.f, +1.f, +1.f, 1.f };
		vertexData[6].position = { -1.f, -1.f, -1.f, 1.f };
		vertexData[7].position = { -1.f, -1.f, +1.f, 1.f };

		vertex_.SetVertexData(vertexData);

		std::array<uint32_t, 6u * 6u> indexData{
			0,1,2,
			2,1,3,	// 右面

			4,5,6,
			6,5,7,	// 左面

			5,0,7,
			7,0,2,	// 前面

			1,4,3,
			3,4,6,	// 後面

			0,5,1,
			1,5,4,	// 上面

			7,2,6,
			6,2,4,	// 下面
		};

		index_.SetIndexData(indexData);
	}
}