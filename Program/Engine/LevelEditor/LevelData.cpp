/// @file LevelData.cpp
/// @brief レベルデータ
/// @author ONUKI seiya
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
		ResourceObjectManager<AssimpData> *const assimpManager = ResourceObjectManager<AssimpData>::GetInstance();
		ResourceObjectManager<ModelData> *const modelManager = ResourceObjectManager<ModelData>::GetInstance();

		// モデルは初期値は"box.obj"であるとする
		const auto defaultModel = modelManager->Load({ assimpManager->Load({ "","box.obj" }) });

		for (const auto &jsonObject : jsonObjectList) {
			assert(jsonObject.contains("type") and "typeコンテナが存在しません");

			// 可視フラグが折れてたら､追加しない
			if (jsonObject.contains("visiblity") and not jsonObject["visiblity"]) { continue; }
			// 種類を取得
			const std::string type = jsonObject["type"];


			// MESHである場合
			//if (type.compare("MESH") == 0 or type.compare("EMPTY") == 0) {

			// コライダかメッシュを持っていない場合は終わる
			if (not jsonObject.contains("file_name") and not jsonObject.contains("collider")) { continue; }

			// オブジェクトを末尾に構築
			auto &objectData = objectDataList.emplace_back();

			// transformの保存
			{
				const auto &transform = jsonObject["transform"];
				{
					const Vector3 scale = transform["scale"].get<Vector3>();

					objectData.transform_.scale_ = type.compare("ARMATURE") == 0 ? Vector3::one : // もし､アーマチュア型なら大きさを無視する
						Vector3{ scale[0], scale[2], scale[1] };
				}
				{
					// 弧度法に変換しつつ､回転量の変換
					const Vector3 rotate = transform["rotation"].get<Vector3>() * -SoLib::Angle::Dig2Rad;

					static const Quaternion kBaseRotate = { 0,1,0,0 };	// Y軸に半回転する

					const Quaternion quate =
						(Quaternion::AnyAxisRotation(Vector3::up, rotate.z)
							* Quaternion::Create({ rotate.x, 0,rotate.y })
							* kBaseRotate).Normalize();

					objectData.transform_.rotate_ = quate;
				}
				{
					const Vector3 translate = transform["translation"].get<Vector3>();

					objectData.transform_.translate_ =
					{ translate[0], translate[2], translate[1] };
				}
			}

			// ファイル名があるならそれを保存する
			if (jsonObject.contains("file_name")) {
				// ファイル名が空でない場合
				if (jsonObject["file_name"] != "") {
					auto assimpData = assimpManager->Load({ .directoryPath_ = jsonObject.contains("directory_name") ? jsonObject["directory_name"] : "" ,. fileName_ = jsonObject["file_name"] });
					// ファイルを読み込めた場合
					if (assimpData) {
						objectData.modelHandle_ = modelManager->Load({ assimpData });
					}
				}
				// モデルの読み込みに失敗していた場合
				if (not objectData.modelHandle_) {
					objectData.modelHandle_ = defaultModel;
				}
			}

			// ファイル名があるならそれを保存する
			if (jsonObject.contains("collider")) {
				const auto &collider = jsonObject["collider"];
				const std::string &colliderType = collider["type"];

				// 型が存在する場合
				if (colliderType != "") {

					if (collider["isTrigger"]) {
						const std::string &funcName = collider["collision_function"];
						objectData.triggerFunc_ = funcName;
					}

					// コライダの形がBoxであった場合
					if (colliderType.compare("BOX")) {
						// コライダのデータを入れ込む
						OBB obb{};

						{
							const Vector3 &transScale = objectData.transform_.scale_;
							const Vector3 scale = collider["scale"].get<Vector3>();

							obb.size = Vector3{ scale[0] * transScale.x, scale[2] * transScale.y, scale[1] * transScale.z } / 2.f;
						}
						{
							const Vector3 center = collider["center"].get<Vector3>();


							obb.centor = { center[0], center[2], center[1] };
						}

						obb.orientations = objectData.transform_.rotate_;
						// データを代入
						objectData.collider_ = obb;
					}
				}
			}

			// 再起関数で走査する
			if (jsonObject.contains("children")) {
				RecursiveLoad(jsonObject["children"], objectData.children_);

			}
			//}

		}
	}
}
