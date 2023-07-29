#pragma once
#include "../../Header/Math/Matrix4x4.h"
#include "../../Header/Math/Vector4.h"
#include "../../Header/Math/Vector3.h"
#include "../../Header/Math/Vector2.h"
#include <wrl.h>
#include <d3d12.h>
#include "../../Header/Math/Transform.h"
#include <array>

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


	Sprite() = default;
	~Sprite() = default;

	static void StaticInit();
private:
	// バッファを生成
	void CreateBuffer();
	// 頂点/添え字をマップ
	void MapVertex();


private:

	ComPtr<ID3D12Resource> vertexResource_ = nullptr;
	VertexData *vertexMap_ = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vbView_ = {};

	ComPtr<ID3D12Resource> indexResource_ = nullptr;
	uint32_t *indexMap_ = nullptr;
	D3D12_INDEX_BUFFER_VIEW ibView_ = {};

	ComPtr<ID3D12Resource> constResource_ = nullptr;
	ConstData *constMap_ = nullptr;

private:

	// マッピング無しTransform
	Transform transform_;

	Vector2 texSize_ = { 1.f,1.f };

	// テクスチャID
	uint32_t textureHaundle_ = 1;

public:

	void SetTextureHaundle(const uint32_t textureHaundle) {
		textureHaundle_ = textureHaundle;
		const D3D12_RESOURCE_DESC &desc = TextureManager::GetInstance()->GetResourceDesc(textureHaundle);
		texSize_ = { float(desc.Width), float(desc.Height) };
	}

	void Init(const std::string &textureName = "white2x2.png");
	void Draw() const;

	[[nodiscard]] static Sprite *const Create();

	void ImGuiWidget();

	static void StartDraw(ID3D12GraphicsCommandList *const commandList);
	static void SetBlendMode(BlendMode blendMode);
	static void EndDraw();
};