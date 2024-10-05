#pragma once
#include "../../Engine/DirectBase/Base/VertexBuffer.h"	// 頂点バッファの型
#include "../../Engine/Utils/Containers/Singleton.h"	// シングルトン型

#include "../Base/RootSignature.h"								// ルートシグネチャ
#include "../../Engine/ResourceObject/ResourceObjectManager.h"	// リソースマネージャ型
#include "../Base/PipelineState.h"								// パイプラインのデータ

#include "NewSprite.h"	// スプライトの型

namespace SolEngine {

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

	private:

		void CreatePipeline(ResourceSource<PipelineState> &pipeline);

	public:
		// 描画時に使うバッファデータ
		std::unique_ptr<BufferData> bufferData_;
		// ルートシグネチャ
		ResourceHandle<RootSignature> rootSignature_;

		std::array<std::unique_ptr<PipelineState>, static_cast<uint32_t>(NewSprite::BlendMode::kCount)> pipeLineState_;

		inline static const char *const kRootParam_ = "b0AL,t0PS";

	private:

		PipelineState &GetPipeline(NewSprite::BlendMode blendMode) { return *pipeLineState_.at(static_cast<uint32_t>(blendMode)); }

	};

}