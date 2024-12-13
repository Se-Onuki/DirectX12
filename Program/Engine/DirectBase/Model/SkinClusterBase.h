/// @file SkinClusterBase.h
/// @brief スキンクラスタの情報源
/// @author ONUKI seiya
#pragma once
#include "../../ResourceObject/ResourceObject.h"
#include "../../ResourceObject/ResourceObjectManager.h"
#include "AssimpData.h"
#include "../../Engine/Utils/Graphics/Color.h"
#include "../Base/CBuffer.h"

struct JointWeightData;

namespace SolEngine {

	class SkinClusterBase : public IResourceObject {
	public:
		// メッシュ内に含まれた､複数のジョイントに対する情報をそれぞれ名前と頂点の重さで保存する
		std::vector<std::unordered_map<std::string/*ジョイント名*/, JointWeightData/*ジョイント情報*/>> skinClusterData_;
	};

	template <>
	class ResourceSource<SkinClusterBase> {
	public:
		ResourceSource<SkinClusterBase>() = default;
		ResourceSource<SkinClusterBase>(const ResourceSource<SkinClusterBase> &) = default;
		ResourceSource<SkinClusterBase> &operator=(const ResourceSource<SkinClusterBase> &) = default;
		ResourceSource<SkinClusterBase>(const ResourceHandle<AssimpData> assimpHandle) : assimpHandle_(assimpHandle) {}

		// ファイルのハンドル
		ResourceHandle<AssimpData> assimpHandle_;

		/// @brief ソースを文字列で表現する
		/// @return ファイル名
		std::string ToStr() const {
			const auto &assimpSource = assimpHandle_.GetSource();
			return assimpSource->directoryPath_ + assimpSource->fileName_;
		}

		bool operator==(const ResourceSource<SkinClusterBase> &) const = default;
	};

	template <>
	class ResourceCreater<SkinClusterBase> {
	public:
		/// @brief リソースを作成する
		/// @param[in] source リソースソース
		/// @return リソース
		std::unique_ptr<SkinClusterBase> CreateObject(const ResourceSource<SkinClusterBase> &source) const;

	};

}

namespace std {

	template<>
	struct hash<SolEngine::ResourceSource<SolEngine::SkinClusterBase>> {
		size_t operator()(const SolEngine::ResourceSource<SolEngine::SkinClusterBase> &data) const {
			return data.assimpHandle_.GetHashID();
		}
	};
}
