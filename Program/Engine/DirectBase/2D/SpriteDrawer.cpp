#include "SpriteDrawer.h"

void SpriteDrawer::BufferData::Generate(BufferData *ptrTarget)
{
	// 頂点バッファを構築する
	ptrTarget->vertex_.SetVertexData(
		std::array<VertexData, 4u>
	{
		{
			{ { 0.f, 1.f, 0.f, 1.f } },	// 左下 { 0, 1 }
			{ { 0.f, 0.f, 0.f, 1.f } },	// 左上 { 0, 0 }
			{ { 1.f, 1.f, 0.f, 1.f } },	// 右下 { 1, 1 }
			{ { 1.f, 0.f, 0.f, 1.f } },	// 右上 { 1, 0 }
		}
	}
	);

	// Indexバッファを構築する
	ptrTarget->index_.SetIndexData(std::array<uint8_t, 6>{2, 0, 1, 1, 3, 2});
	///
	/// １→３
	///	↑　↓
	/// ０←２
	///

}

void SpriteDrawer::Init()
{
	// バッファデータの確保
	bufferData_ = std::make_unique<BufferData>();
	// バッファデータの構築
	BufferData::Generate(bufferData_.get());

}
