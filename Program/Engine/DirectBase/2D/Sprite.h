/// @file Sprite.h
/// @brief スプライトの実装
/// @author ONUKI seiya

#pragma once
#include "../../Engine/Utils/Graphics/Color.h"
#include "../../Engine/Utils/Math/Matrix4x4.h"
#include "../../Engine/Utils/Math/Transform.h"
#include "../../Engine/Utils/Math/Vector2.h"
#include "../../Engine/Utils/Math/Vector3.h"
#include "../../Engine/Utils/Math/Vector4.h"
#include "../File/VariantItem.h"
#include <array>
#include <d3d12.h>
#include <wrl.h>

#include "../Base/CBuffer.h"
#include "../Base/VertexBuffer.h"

#include "../Base/LeakChecker.h"
#include "../Base/TextureManager.h"

/// @class Sprite
/// @brief スプライト
class Sprite
{

	/// @brief ComPtrのエイリアス
	/// @tparam T 適用する型
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
	// 透視投影行列
	static Matrix4x4 matProjection_;
	// [借用] デバイス
	static ID3D12Device *device_;
	// [借用] コマンドリスト
	static ID3D12GraphicsCommandList *commandList_;

	/// @fn void CreatePipeLine(void)
	/// @brief パイプラインの作成
	static void CreatePipeLine();

public:
	/// @enum BlendMode
	/// @brief ブレンドモード一覧
	enum class BlendMode
	{
		kNone,	   // ブレンド無し
		kNormal,   // αブレンド
		kAdd,	   // 加算合成
		kSubtract, // 減算合成
		kMultily,  // 乗算合成
		kScreen,   // スクリーン合成

		kTotal // 総数
	};

private:
	static DirectResourceLeakChecker leakChecker; // リークチェッカー

	/// @brief パイプラインステート
	static std::array<ComPtr<ID3D12PipelineState>, (uint32_t)BlendMode::kTotal> graphicsPipelineState_;
	/// @brief ルートシグネチャ
	static ComPtr<ID3D12RootSignature> rootSignature_;

public:
	/// @struct VertexData
	/// @brief 頂点情報
	struct VertexData
	{
		/// @brief 頂点座標
		VariantItem<"Position", Vector4> position;
		/// @brief uv座標
		VariantItem<"TexCoord", Vector2> texCoord;
	};

	/// @struct ConstData
	/// @brief 定数バッファ情報
	struct ConstData
	{
		/// @brief カラー
		VariantItem<"Color", SoLib::Color::RGB4> color;
		/// @brief ワールド行列
		Matrix4x4 matWorldProjection;
	};

	/// @enum RootParameter
	/// @brief ルートパラメータ
	enum class RootParameter
	{
		kTexture,  // テクスチャ
		kConstData // スプライト情報
	};

	/// @enum VertexNumer
	/// @brief 頂点番号
	enum class VertexNumer
	{
		LDown,
		LTop,
		RDown,
		RTop
	};

	Sprite() = default;
	~Sprite() = default;

	/// @fn static void StaticInit(void)
	/// @brief 静的初期化
	static void StaticInit();

	/// @fn static void Finalize(void)
	/// @brief 終了
	static void Finalize();

private:
	/// @fn void CreateBuffer(void)
	/// @brief バッファを生成
	void CreateBuffer();

	/// @fn void CalcBuffer(void)
	/// @brief 頂点/添え字をマップ
	void MapVertex();

	/// @fn void Init(const std::string &)
	/// @brief 初期化
	/// @param[in] textureName
	void Init(const std::string &textureName = "white2x2.png");
	/// @fn void Init(const uint32_t)
	/// @brief 初期化
	/// @param[in] textureHaundle
	void Init(const uint32_t textureHaundle);

private:
	// 頂点データ
	VertexBuffer<VertexData> vertexData_;
	// Indexデータ
	inline static std::unique_ptr<IndexBuffer<uint32_t>> indexData_;
	/// @brief 定数バッファ
	CBuffer<ConstData> constData_;

private:
	// Transform
	SimpleTransformEuler transform_;
	/// @brief アフィン行列
	Matrix4x4 transMat_ = Matrix4x4::Identity();

private:
	// テクスチャ原点 : 差分
	std::pair<Vector2, Vector2> uv_{};
	// 中心点
	Vector2 pivot_{};

	// テクスチャID
	TextureHandle textureHaundle_ = 1u;

	/// @brief スプライトの表示状態
	bool isVisible_ = true;

public:
	/// @fn void SetTextureHaundle(const uint32_t)
	/// @brief テクスチャ設定
	/// @param[in] textureHaundle テクスチャハンドル
	void SetTextureHaundle(const uint32_t textureHaundle)
	{
		textureHaundle_ = textureHaundle;
		SetTexOrigin(ZeroVector2);
		SetTexDiff(textureHaundle_->textureSize_);
	}

	/// @fn void SetTexOrigin(const Vector2)
	/// @brief テクスチャ原点設定(左上)
	/// @param[in] texOrigin テクスチャの原点
	void SetTexOrigin(const Vector2 texOrigin);

	/// @fn void SetTexDiff(const Vector2)
	/// @brief テクスチャ差分設定(右下)
	/// @param[in] texDiff テクスチャの差分
	void SetTexDiff(const Vector2 texDiff);

	/// @fn const std::pair<Vector2, Vector2> &GetUV(void)
	/// @brief テクスチャ座標取得
	/// @return { テクスチャ原点(左上), テクスチャ差分(右下) }
	const std::pair<Vector2, Vector2> &GetUV() const { return uv_; }

	/// @fn void SetVisible(bool)
	/// @brief 表示状態設定
	/// @param[in] visible 表示状態
	void SetVisible(bool visible) { isVisible_ = visible; }
	/// @fn bool GetVisible(void)
	/// @brief 表示状態取得
	/// @return 表示中ならtrue
	bool GetVisible() const { return isVisible_; }

	/// @fn void SetPivot(const Vector2)
	/// @brief 中心点設定
	/// @param[in] pivot 中心点
	void SetPivot(const Vector2 pivot);

	/// @fn void SetScale(const Vector2)
	/// @brief スケール設定
	/// @param[in] scale スケール
	void SetScale(const Vector2 scale);
	/// @fn void SetRotate(const float)
	/// @brief 角度の設定
	/// @param[in] angle 角度
	void SetRotate(const float angle);
	/// @fn void SetPosition(const Vector2)
	/// @brief 座標設定
	/// @param position 座標
	void SetPosition(const Vector2 position);

	/// @fn void SetTransform(const Transform2D)
	/// @brief スプライトの姿勢設定
	/// @param[in] trans スプライトの姿勢
	void SetTransform(const Transform2D &trans);

	/// @fn const Transform2D &GetTransform(void)
	/// @brief 姿勢取得
	/// @return スプライトの姿勢
	const auto &GetTransform() const { return transform_; }

	/// @fn void SetColor(const SoLib::Color::RGB4 &)
	/// @brief 色設定
	/// @param[in] color カラー
	void SetColor(const SoLib::Color::RGB4 &color);

	/// @fn const SoLib::Color::RGB4 &GetColor(void)
	/// @brief 色取得
	/// @return カラー
	const SoLib::Color::RGB4 &GetColor() const;

	/// @fn TextureHandle GetTexHandle(void)
	/// @brief テクスチャハンドル取得
	/// @return テクスチャハンドル
	TextureHandle GetTexHandle() const noexcept { return textureHaundle_; }

	/// @fn void Draw(void)
	/// @brief 描画の実行
	void Draw() const;

	/// @fn static std::unique_ptr<Sprite> Generate(void)
	/// @brief テクスチャの生成
	/// @return 生成されたテクスチャ
	[[nodiscard]] static std::unique_ptr<Sprite> Generate();
	/// @param[in] textureHaundle テクスチャハンドル
	[[nodiscard]] static std::unique_ptr<Sprite> Generate(const uint32_t textureHaundle);
	/// @param[in] position 座標
	/// @param[in] scale スケール
	[[nodiscard]] static std::unique_ptr<Sprite> Generate(const uint32_t textureHaundle, const Vector2 position, const Vector2 scale);

	/// @fn void ImGuiWidget(void)
	/// @brief ImGuiウィジェットの表示
	void ImGuiWidget();

	/// @fn void CalcBuffer(void)
	/// @brief 頂点データの計算
	void CalcBuffer();

	/// @fn static void StartDraw(ID3D12GraphicsCommandList *const)
	/// @brief スプライト描画の開始
	/// @param[in] commandList コマンドリストの借用
	static void StartDraw(ID3D12GraphicsCommandList *const commandList);
	/// @fn static void SetBlendMode(BlendMode)
	/// @brief ブレンドモードの設定
	/// @param[in] blendMode ブレンドモード
	static void SetBlendMode(BlendMode blendMode);
	/// @fn static void EndDraw(void)
	/// @brief スプライト描画の終了
	static void EndDraw();
};