#include "LevelData.h"
#include <fstream>

namespace SolEngine {
	/// @brief レベルデータを構築する関数
	/// @param source レベルデータのソースデータファイル { 文字列型でファイル名が入る }
	/// @return 構築されたレベルデータ
	std::unique_ptr<LevelData> ResourceCreater<LevelData>::CreateObject(const ResourceSource<LevelData> &source) const {

		// ファイルパスの構築
		const std::string fullPath = kDefaultDirectory_ + source.fileName_;

		// ファイルストリーム
		std::ifstream ifs;

		// ファイルオープン
		ifs.open(fullPath);

		if (ifs.fail()) {
			assert(0 and "レベルデータファイルへのアクセスに失敗しました");
		}

		// jsonデータ
		nlohmann::json deserialized;

		// 読み取り
		ifs >> deserialized;

		// ファイルを閉じる
		ifs.close();

		// ファイルデータが正規のものか検証
		assert(deserialized.is_object() and "jsonデータが格納されていません");
		assert(deserialized.contains("name") and "\"name \"が存在しません");
		assert(deserialized["name"].is_string() and "\"name\"から文字列を取得できません");

		const std::string name = deserialized["name"];

		assert(name.compare("scene") == 0 and "レベル名が正しくありません");

		// 構築されたレベルデータのインスタンス
		std::unique_ptr<LevelData> levelData = std::make_unique<LevelData>();

		// "objects"の全オブジェクトを走査
		const auto &jsonObjectList = deserialized["objects"];

		RecursiveLoad(jsonObjectList, levelData->objectList_);


		return std::move(levelData);
	}

	void ResourceCreater<LevelData>::RecursiveLoad(const nlohmann::json &jsonObjectList, std::list<LevelData::ObjectData> &objectDataList) const
	{
		for (const auto &jsonObject : jsonObjectList) {
			assert(jsonObject.contains("type") and "typeコンテナが存在しません");

			// 種類を取得
			const std::string type = jsonObject["type"];

			// MESHである場合
			if (type.compare("MESH") == 0) {
				// オブジェクトを末尾に構築
				auto &objectData = objectDataList.emplace_back();

				// ファイル名があるならそれを保存する
				if (jsonObject.contains("file_name")) {
					objectData.fileName_ = jsonObject["file_name"];
				}

				// transformの保存
				{
					const auto &transform = jsonObject["transform"];
					{
						const Vector3 scale = transform["scale"].get<Vector3>();

						objectData.transform_.scale_ =
						{ scale[0], scale[2], scale[1] };
					}
					{
						const Vector3 rotate = transform["rotation"].get<Vector3>();

						objectData.transform_.rotate_ =
							-Vector3{ rotate[0], rotate[2], rotate[1] };
					}
					{
						const Vector3 translate = transform["translation"].get<Vector3>();

						objectData.transform_.translate_ =
						{ translate[0], translate[2], translate[1] };
					}
				}

				// 再起関数で走査する
				if (jsonObject.contains("children")) {
					RecursiveLoad(jsonObject["children"], objectData.children_);

				}
			}

		}
	}
}
