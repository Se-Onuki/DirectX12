#pragma once
#include "../../ResourceObject/ResourceObject.h"
#include "../../ResourceObject/ResourceObjectManager.h"
#include "AssimpData.h"
#include "../../Engine/Utils/Graphics/Color.h"
#include "../Base/CBuffer.h"
#include "Model.h"

namespace SolEngine {

	class Material : public IResourceObject {
	public:

		enum class BlendMode : uint32_t {
			kNone,		// ブレンド無し
			kNormal,	// αブレンド
			kAdd,		// 加算合成
			kSubtract,	// 減算合成
			kMultily,	// 乗算合成
			kScreen,	// スクリーン合成

			kMaxSize,
		};

		struct MaterialData {
			SoLib::Color::RGB4 color;		// 色(RGBA)
			SoLib::Color::RGB4 emissive;    // 自己発光色(RGBA)
			SoLib::Color::RGB4 ambient;    // アンビエントカラー(RGBA)
			Matrix4x4 uvTransform;

			float shininess = { 1.f };
			float shininessStrength = 0.f;
		};

		CBuffer<MaterialData> materialData_;
		Model::BlendMode blendMode_ = Model::BlendMode::kNone;

		uint32_t texHandle_;
	};

	template <>
	class ResourceSource<Material> {
	public:
		// ファイルのハンドル
		ResourceObjectManager<AssimpData>::Handle assimpHandle_;
		// materialのmodel内Index
		uint32_t index_;

		std::string ToStr() const {
			const auto *const assimpSource = assimpHandle_.GetSource();
			return assimpSource->directoryPath_ + assimpSource->fileName_ + '[' + std::to_string(index_) + ']';
		}

		bool operator==(const ResourceSource<Material> &) const = default;
	};

	template <>
	class ResourceCreater<Material> {
	public:

		std::unique_ptr<Material> CreateObject(const ResourceSource<Material> &source) const;

	private:
		/// @brief パラメータの取得
		/// @tparam T 型名
		/// @param material 元となるマテリアル
		/// @param target 書き込む先
		template<SoLib::IsRealType T>
		static void GetParam(const aiMaterial *material, const char *pKey, unsigned int type, unsigned int idx, T &target) {
			if (T tmp; material->Get(pKey, type, idx, tmp) == AI_SUCCESS) { target = tmp; }
		}

	};

}

namespace std {

	template<>
	struct hash<SolEngine::ResourceSource<SolEngine::Material>> {
		size_t operator()(const SolEngine::ResourceSource<SolEngine::Material> &data) const {
			return size_t{ static_cast<size_t>(data.assimpHandle_.GetHandle()) << 32u | static_cast<size_t>(data.index_) };
		}
	};
}

namespace SoLib {
	template <>
	bool ImGuiWidget<SolEngine::Material>(const char *const label, SolEngine::Material *const value);

}
