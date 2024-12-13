/// @file AssimpData.h
/// @brief Assimpのデータの保持
/// @author ONUKI seiya
#pragma once
#include "../../ResourceObject/ResourceObject.h"
#include <assimp/scene.h>
#include <assimp/Importer.hpp>

namespace SolEngine {

	/// @class AssimpData
	/// @brief Assimpのデータの保持クラス
	class AssimpData : public IResourceObject {
	public:
		/// @brief 頂点に対応するマテリアルの数
		inline static constexpr uint32_t kMaterialCountByVertex_ = 8u;

		// assimpのローダ
		std::unique_ptr<Assimp::Importer> importer_;	// こいつが存在する = シーンデータが保存される

		/// @brief リソースの破棄
		void Destroy() {
			importer_.reset();
		}

		/// @brief リソースの有無
		operator bool() const { return importer_.get(); }
	};

	template <>
	class ResourceSource<AssimpData> {
	public:
		// ディレクトリへのパス
		std::string directoryPath_;
		// ファイルの名前
		std::string fileName_;

		bool operator==(const ResourceSource &) const = default;
	};

	template <>
	class SolEngine::ResourceCreater<AssimpData> {
	public:

		/// @brief オブジェクトの生成
		/// @param[in] source リソースソース
		/// @return 生成されたリソース
		std::unique_ptr<AssimpData> CreateObject(const SolEngine::ResourceSource<AssimpData> &source) const;


	private:


	};
}

namespace std {

	template<>
	struct hash<SolEngine::ResourceSource<SolEngine::AssimpData>> {
		size_t operator()(const SolEngine::ResourceSource<SolEngine::AssimpData> &data) const {
			return std::hash<std::string>()(data.directoryPath_ + '/' + data.fileName_);
		}
	};
}

