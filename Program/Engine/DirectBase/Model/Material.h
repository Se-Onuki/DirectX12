/// @file Material.h
/// @brief マテリアルクラス
/// @author ONUKI seiya
#pragma once
#include "../../Engine/Utils/Graphics/Color.h"
#include "../../ResourceObject/ResourceObject.h"
#include "../../ResourceObject/ResourceObjectManager.h"
#include "../Base/CBuffer.h"
#include "AssimpData.h"
#include "Model.h"

namespace SolEngine {

	class Material : public IResourceObject
	{
	public:
		/// @enum BlendMode
		/// @brief ブレンドモード
		enum class BlendMode : uint32_t
		{
			kNone,	   // ブレンド無し
			kNormal,   // αブレンド
			kAdd,	   // 加算合成
			kSubtract, // 減算合成
			kMultily,  // 乗算合成
			kScreen,   // スクリーン合成

			kMaxSize, // 最大数
		};

		/// @struct MaterialData
		/// @brief 転送用構造体
		struct MaterialData
		{
			SoLib::Color::RGB4 color;	 // 色(RGBA)
			SoLib::Color::RGB4 emissive; // 自己発光色(RGBA)
			SoLib::Color::RGB4 ambient;	 // アンビエントカラー(RGBA)
			Matrix4x4 uvTransform;		 // UV変換行列

			float shininess = {1.f};	   // 鏡面反射度
			float shininessStrength = 0.f; // 鏡面反射強度
		};

		CBuffer<MaterialData> materialData_;
		Model::BlendMode blendMode_ = Model::BlendMode::kNone;

		uint32_t texHandle_;
	};

	template <>
	class ResourceSource<Material>
	{
	public:
		// ファイルのハンドル
		ResourceObjectManager<AssimpData>::Handle assimpHandle_;
		// materialのmodel内Index
		uint32_t index_;

		/// @brief 文字列化
		/// @return 文字列
		std::string ToStr() const
		{
			const auto *const assimpSource = assimpHandle_.GetSource();
			return assimpSource->directoryPath_ + assimpSource->fileName_ + '[' + std::to_string(index_) + ']';
		}

		bool operator==(const ResourceSource<Material> &) const = default;
	};

	template <>
	class ResourceCreater<Material>
	{
	public:
		/// @brief オブジェクトの生成
		/// @param[in] source リソースソース
		/// @return 生成されたリソース
		std::unique_ptr<Material> CreateObject(const ResourceSource<Material> &source) const;

	private:
		/// @brief パラメータの取得
		/// @tparam T 型名
		/// @param[in] material 元となるマテリアル
		/// @param[in] target 書き込む先
		template <SoLib::IsRealType T>
		static void GetParam(const aiMaterial *material, const char *pKey, unsigned int type, unsigned int idx, T &target)
		{
			if (T tmp; material->Get(pKey, type, idx, tmp) == AI_SUCCESS) {
				target = tmp;
			}
		}
	};

}

namespace std {

	template <>
	struct hash<SolEngine::ResourceSource<SolEngine::Material>>
	{
		size_t operator()(const SolEngine::ResourceSource<SolEngine::Material> &data) const
		{
			return data.assimpHandle_.GetHashID();
		}
	};
}

namespace SoLib {
	/// @brief ImGuiのウィジェット
	/// @param label 表示するラベル
	/// @param value 表示する値
	/// @return 変更されたらtrue
	template <>
	bool ImGuiWidget<SolEngine::Material>(const char *const label, SolEngine::Material *const value);

}
