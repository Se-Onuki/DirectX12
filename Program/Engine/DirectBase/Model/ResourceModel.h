#pragma once
#include "ModelData.h"
#include "Mesh.h"
#include <unordered_map>

namespace SolEngine {

	/// @brief リソースインターフェース
	namespace ResourceInterface {

		/// @brief リソースの基底クラス
		class IResource {
		public:
			/// @brief 仮想デストラクタ
			virtual ~IResource() {}

			/// @brief 名前の設定
			/// @param name 設定する名前
			void SetName(const std::string &name) { resourceName_ = name; }
			void SetName(std::string &&name) { resourceName_ = std::move(name); }
			/// @brief 名前の取得
			/// @return リソース名
			const std::string &GetName() const { return resourceName_; }

			/// @brief ロードが完了しているか
			/// @return 完了しているならtrue
			virtual bool IsLoaded() const { return true; }

		private:
			std::string resourceName_;
		};

		template<typename T>
		concept IsIResource = std::is_base_of_v<IResource, T>;

		template<IsIResource T>
		class ResourceLoader;

		class ResourceMap {
		public:

			using MapStrResource = std::unordered_map<std::string, std::unique_ptr<IResource>>;

			using ResourceInstance = MapStrResource::const_iterator;

		private:

			template <bool IsConst>
			using IsSucceedResource = std::pair<std::conditional_t<IsConst, MapStrResource::const_iterator, MapStrResource::iterator>, bool>;

		public:


			/// @brief リソースデータの追加
			/// @param resource リソースデータ
			void AddResource(std::unique_ptr<IResource> &&resource);

			/// @brief リソースデータの検索
			/// @param name リソース名
			/// @return リソースへのアクセスイテレータ
			ResourceInstance Find(const std::string &name) const;


		private:

			///// @brief リソースデータの追加
			///// @param resource リソースデータ
			//IsSucceedResource<false> InnerAddResource(std::unique_ptr<IResource> &&resource);
			// リソースデータのマップデータ
			std::unordered_map<std::string, std::unique_ptr<IResource>> resourceMap_;

		};


	}

	/// @brief リソースデータ
	namespace Resource {

		struct ResourceStruct;

		/// @brief モデルデータ
		class Model : public ResourceInterface::IResource {
		public:
			~Model() override = default;
			//std::vector<ResourceHandle<Mesh>> mesh_;

			ResourceHandle<ModelData> model_;

		};


	}

	namespace ResourceInterface {
		template<>
		class ResourceLoader<Resource::Model> {
		public:
			ResourceLoader() = default;

			/// @brief ロード完了したかどうか
			/// @return 完了したのならtrue
			bool IsLoaded() const;

			std::unique_ptr<Resource::Model> Generate(const SolEngine::Resource::ResourceStruct &resource);


		private:


		};
	}

}