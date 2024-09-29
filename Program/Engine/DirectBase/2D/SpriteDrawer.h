#pragma once
#include "../../Engine/DirectBase/Base/VertexBuffer.h"	// 頂点バッファの型
#include "../../Utils/Containers/Singleton.h"

/// @brief スプライトの描画処理を行う
class SpriteDrawer : public SoLib::Singleton<SpriteDrawer> {

	SpriteDrawer() = default;
	SpriteDrawer(const SpriteDrawer &) = delete;
	SpriteDrawer &operator=(const SpriteDrawer &) = delete;
	~SpriteDrawer() = default;

	friend SoLib::Singleton<SpriteDrawer>;

	///
	///	情報などの定義
	///
public:

	/// @brief 描画に使うデータなど
	class BufferData {
	public:

		enum class VertexNumber {
			LDown,
			LTop,
			RDown,
			RTop
		};

		static void Generate(BufferData *ptrTarget);

		/// @brief 頂点バッファの型
		struct VertexData {
			Vector4 position_;	// 座標
		};

	public:
		// 頂点バッファ
		VertexBuffer<VertexData> vertex_;
		// Indexバッファ
		IndexBuffer<uint8_t> index_;

	};

public:
	/// @brief 初期化処理
	void Init();

public:
	// 描画時に使うバッファデータ
	std::unique_ptr<BufferData> bufferData_;

};