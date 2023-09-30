#pragma once
#include "../../Utils/Math/Matrix4x4.h"
#include "../../Utils/Math/Vector4.h"
#include "../../Utils/Math/Vector3.h"
#include "../../Utils/Math/Vector2.h"
#include <wrl.h>
#include <d3d12.h>
#include "../../Utils/Math/Transform.h"
#include <array>

#include "../Base/CBuffer.h"

#include "../Base/LeakChecker.h"
#include "../Base/TextureManager.h"

class Sprite {

	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// 透視投影行列
	static Matrix4x4 matProjection_;
	// [借用] デバイス
	static ID3D12Device *device_;
	// [借用] コマンドリスト
	static ID3D12GraphicsCommandList *commandList_;

	static void CreatePipeLine();

public:
	enum class BlendMode {
		kNone,		// ブレンド無し
		kNormal,	// αブレンド
		kAdd,		// 加算合成
		kSubtract,	// 減算合成
		kMultily,	// 乗算合成
		kScreen,	// スクリーン合成

		kTotal	// 総数
	};
private:

	static DirectResourceLeakChecker leakChecker; // リークチェッカー

	static std::array<ComPtr<ID3D12PipelineState>, (uint32_t)BlendMode::kTotal> graphicsPipelineState_;
	static ComPtr<ID3D12RootSignature> rootSignature_;


public:
	struct VertexData {
		Vector4 position;
		Vector2 texCoord;
	};

	struct ConstData {
		Vector4 color;
		Matrix4x4 matWorldProjection;
	};

	enum class RootParameter {
		kTexture,	// テクスチャ
		kConstData	// スプライト情報
	};

	enum class VertexNumer {
		LDown,
		LTop,
		RDown,
		RTop
	};

	Sprite() = default;
	~Sprite() = default;

	static void StaticInit();
private:
	// バッファを生成
	void CreateBuffer();
	// 頂点/添え字をマップ
	void MapVertex();


	void Init(const std::string &textureName = "white2x2.png");
	void Init(const uint32_t &textureHaundle);

private:

	// 頂点データ
	VertexCBuffer<VertexData, true> vertexData_;

	CBuffer<ConstData> constData_;

private:

	// マッピング無しTransform
	Transform transform_;

private:

	// テクスチャ原点 : 差分
	std::pair<Vector2, Vector2> uv_{};
	// 中心点
	Vector2 pivot_{};

	bool isVisible_ = true;


	// テクスチャID
	uint32_t textureHaundle_ = 1u;

	// テクスチャリソースの情報
	D3D12_RESOURCE_DESC resourceDesc{};

public:
	/// @brief テクスチャ設定
	/// @param textureHaundle テクスチャハンドル
	void SetTextureHaundle(const uint32_t &textureHaundle) {
		textureHaundle_ = textureHaundle;
		resourceDesc = TextureManager::GetInstance()->GetResourceDesc(textureHaundle);
		SetTexOrigin(ZeroVector2);
		SetTexDiff(Vector2{ float(resourceDesc.Width), float(resourceDesc.Height) });
	}

	/// @brief テクスチャ原点設定(左上)
	void SetTexOrigin(const Vector2 &texOrigin);

	/// @brief テクスチャ差分設定(右下)
	void SetTexDiff(const Vector2 &texDiff);

	/// @brief テクスチャ座標取得 
	/// @return { テクスチャ原点(左上), テクスチャ差分(右下) }
	const std::pair<Vector2, Vector2> &GetUV() const { return uv_; }

	void SetVisible(bool visible) { isVisible_ = visible; }
	bool GetVisible() const { return isVisible_; }

	/// @brief 中心点設定
	void SetPivot(const Vector2 &pivot);

	void SetScale(const Vector2 &scale);
	void SetRotate(const float angle);
	void SetPosition(const Vector2 &position);

	const Transform &GetTransform() const { return transform_; }

	void SetColor(const Vector4 &color);
	const Vector4 &GetColor()const;


	/// @brief 描画
	void Draw() const;

	[[nodiscard]] static Sprite *const Create();
	[[nodiscard]] static Sprite *const Create(const uint32_t textureHaundle);
	[[nodiscard]] static Sprite *const Create(const uint32_t textureHaundle, const Vector2 &position, const Vector2 &scale);

	void ImGuiWidget();

	/// @brief 頂点データ編集
	void CalcBuffer();

	static void StartDraw(ID3D12GraphicsCommandList *const commandList);
	static void SetBlendMode(BlendMode blendMode);
	static void EndDraw();
};