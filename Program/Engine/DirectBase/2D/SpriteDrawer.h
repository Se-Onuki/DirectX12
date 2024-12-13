/// @file SpriteDrawer.h
/// @brief スプライトの描画処理クラス
/// @author ONUKI seiya
#pragma once
#include "../../Engine/DirectBase/Base/VertexBuffer.h" // 頂点バッファの型
#include "../../Engine/Utils/Containers/Singleton.h"   // シングルトン型

#include "../../Engine/ResourceObject/ResourceObjectManager.h" // リソースマネージャ型
#include "../Base/PipelineState.h"							   // パイプラインのデータ
#include "../Base/RootSignature.h"							   // ルートシグネチャ

#include "NewSprite.h" // スプライトの型

namespace SolEngine {

	/// @class SpriteDrawer
	/// @brief スプライトの描画処理を行う
	class SpriteDrawer : public SoLib::Singleton<SpriteDrawer>
	{

		SpriteDrawer() = default;
		SpriteDrawer(const SpriteDrawer &) = delete;
		SpriteDrawer &operator=(const SpriteDrawer &) = delete;
		~SpriteDrawer() = default;

		friend SoLib::Singleton<SpriteDrawer>;

		///
		///	情報などの定義
		///
	public:
		/// @class BufferData
		/// @brief 描画に使うデータなど
		class BufferData
		{
		public:
			/// @enum VertexNumber
			/// @brief 頂点番号
			enum class VertexNumber
			{
				LDown,
				LTop,
				RDown,
				RTop
			};
			/// @fn static void Generate(BufferData *)
			/// @brief BufferDataの生成
			/// @param[in] ptrTarget 書き込み先
			static void Generate(BufferData *ptrTarget);

			/// @struct VertexData
			/// @brief 頂点バッファの型
			struct VertexData
			{
				Vector4 position_; // 座標
			};

		public:
			// 頂点バッファ
			VertexBuffer<VertexData> vertex_;
			// Indexバッファ
			IndexBuffer<uint8_t> index_;
		};

	public:
		/// @fn void Init(void)
		/// @brief 初期化処理
		void Init();

	private:
		/// @fn void CreatePipeline(ResourceSource<PipelineState> &)
		/// @brief パイプラインの生成
		/// @param[in] pipeline パイプライン
		void CreatePipeline(ResourceSource<PipelineState> &pipeline);

	public:
		// 描画時に使うバッファデータ
		std::unique_ptr<BufferData> bufferData_;
		// ルートシグネチャ
		ResourceHandle<RootSignature> rootSignature_;
		/// @brief パイプラインステート
		std::array<std::unique_ptr<PipelineState>, static_cast<uint32_t>(NewSprite::BlendMode::kCount)> pipeLineState_;
		/// @brief ルートパラメータの引数
		inline static const char *const kRootParam_ = "b0AL,t0PS";

	private:
		/// @fn PipelineState &GetPipeline(NewSprite::BlendMode)
		/// @brief パイプラインの取得
		/// @param[in] blendMode ブレンドモード
		/// @return パイプライン
		PipelineState &GetPipeline(NewSprite::BlendMode blendMode) { return *pipeLineState_.at(static_cast<uint32_t>(blendMode)); }
	};

}