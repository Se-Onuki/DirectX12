#include "GlobalVariables.h"
// #include <WinUser.h>
#include <fstream>
#include <imgui.h>
#include <json.hpp>
#include <windows.h>

#include "../../DirectBase/Base/WinApp.h"

const GlobalVariables::Item &GlobalVariables::Get(const std::string &groupName, const std::string &key) const {
	// グループ内を検索
	auto itGroup = datas_.find(groupName);
	assert(itGroup != datas_.end());

	// キーがあるか
	auto itItem = itGroup->second.find(key);
	assert(itItem != itGroup->second.end());

	return itItem->second;
}

void GlobalVariables::Update() {
	if (!ImGui::Begin("GlobalVariables", nullptr, ImGuiWindowFlags_MenuBar)) {
		ImGui::End();
		return;
	}
	if (!ImGui::BeginMenuBar()) {
		return;
	}

	for (auto itGroup = datas_.begin(); itGroup != datas_.end(); itGroup++) {
		const std::string &groupName = itGroup->first;

		Group &group = itGroup->second;

		if (!ImGui::BeginMenu(groupName.c_str())) {
			continue;
		}
		for (auto itItem = group.begin(); itItem != group.end(); itItem++) {
			const std::string &itemName = itItem->first;

			Item &item = itItem->second;

			if (std::holds_alternative<int32_t>(item)) {
				int32_t *ptr = std::get_if<int32_t>(&item);
				ImGui::SliderInt(itemName.c_str(), ptr, 0, 100);
			}
			else if (std::holds_alternative<float>(item)) {
				float *ptr = std::get_if<float>(&item);
				ImGui::SliderFloat(itemName.c_str(), ptr, 0.f, 100.f);
			}
			else if (std::holds_alternative<Vector2>(item)) {
				Vector2 *ptr = std::get_if<Vector2>(&item);
				ImGui::SliderFloat2(itemName.c_str(), &ptr->x, -10.f, -10.f);
			}
			else if (std::holds_alternative<Vector3>(item)) {
				Vector3 *ptr = std::get_if<Vector3>(&item);
				ImGui::SliderFloat3(itemName.c_str(), &ptr->x, -10.f, -10.f);
			}
			else if (std::holds_alternative<Vector4>(item)) {
				Vector4 *ptr = std::get_if<Vector4>(&item);
				ImGui::SliderFloat4(itemName.c_str(), &ptr->x, -10.f, -10.f);
			}
		}

		ImGui::Text("\n");
		if (ImGui::Button("Save")) {

			SaveFile(groupName);
			std::string message = std::format("{}.json saved", groupName);
			MessageBoxA(nullptr, message.c_str(), "GlobalVariables", 0);
		}

		ImGui::EndMenu();
	}
	ImGui::EndMenuBar();
	ImGui::End();
}

void GlobalVariables::SaveFile(const std::string &groupName) const {
	// グループ内を検索
	auto itGroup = datas_.find(groupName);
	assert(itGroup != datas_.end());

	nlohmann::json root;                        // ""空
	root = nlohmann::json::object();            // "{}" キー無しのJson構造体を作成
	root[groupName] = nlohmann::json::object(); // "{"groupName":{}}"オブジェクト生成

	for (auto itItem = itGroup->second.begin(); itItem != itGroup->second.end(); itItem++) {
		const std::string &itemName = itItem->first; // キー
		const Item &item = itItem->second;           // Value

		if (std::holds_alternative<int32_t>(item)) {
			root[groupName][itemName] = std::get<int32_t>(item);
		}
		else if (std::holds_alternative<float>(item)) {
			root[groupName][itemName] = std::get<float>(item);
		}
		else if (std::holds_alternative<Vector2>(item)) {
			const Vector2 &value = std::get<Vector2>(item);
			root[groupName][itemName] = nlohmann::json::array({ value.x, value.y });
		}
		else if (std::holds_alternative<Vector3>(item)) {
			const Vector3 &value = std::get<Vector3>(item);
			root[groupName][itemName] = nlohmann::json::array({ value.x, value.y, value.z });
		}
		else if (std::holds_alternative<Vector4>(item)) {
			const Vector4 &value = std::get<Vector4>(item);
			root[groupName][itemName] = nlohmann::json::array({ value.x, value.y, value.z, value.w });
		}
	}

	// ディレクトリが無ければ作成する
	std::filesystem::path dir{ kDirectoryPath };
	if (!std::filesystem::exists(kDirectoryPath)) {
		std::filesystem::create_directory(kDirectoryPath);
	}

	// 書き込み対象のJsonファイルを作成する
	std::string filePath = kDirectoryPath + groupName + ".json";
	std::ofstream ofs;
	ofs.open(filePath);

	// ファイルオープン失敗の時
	if (ofs.fail()) {
		std::string message = "Failed open data file for write.";
		MessageBoxA(nullptr, message.c_str(), "GlobalVariables", 0);
		assert(0);
		return;
	}

	// ファイルにjson文字列を書き込む(インデント幅4)
	ofs << std::setw(4) << root << std::endl;
	ofs.close();
}

void GlobalVariables::LoadFile() {
	// ディレクトリが無ければ終了
	std::filesystem::path dir{ kDirectoryPath };
	if (!std::filesystem::exists(kDirectoryPath)) {
		return;
	}

	std::filesystem::directory_iterator dir_it(kDirectoryPath);
	for (const std::filesystem::directory_entry &entry : dir_it) {
		// ファイルパスを取得
		const std::filesystem::path &filePath = entry.path();

		// ファイル拡張子を取得
		std::string extension = filePath.extension().string();
		// .jsonファイル以外はスキップ
		if (extension.compare(".json") != 0) {
			continue;
		}
		LoadFile(filePath.stem().string());
	}
}

void GlobalVariables::LoadFile(const std::string &groupName) {
	// 読み込むJSONファイルのフルパスを合成する
	std::string filePath = kDirectoryPath + groupName + ".json";
	// 読み込み用ファイルストリーム
	std::ifstream ifs;
	// ファイルを読み込み用に開く
	ifs.open(filePath);

	// ファイルオープンエラー
	if (ifs.fail()) {
		std::string message = "Failed open data file for read.";
		MessageBoxA(nullptr, message.c_str(), "GlobalVariables", 0);
		assert(0);
		return;
	}

	nlohmann::json root;
	ifs >> root;
	ifs.close();

	// グループを検索
	nlohmann::json::iterator itGroup = root.find(groupName);
	assert(itGroup != root.end());

	for (nlohmann::json::iterator itItem = itGroup->begin(); itItem != itGroup->end(); itItem++) {
		// アイテム名を取得
		const std::string &itemName = itItem.key();

		if (itItem->is_number_integer()) {
			// int型の値
			int32_t value = itItem->get<int32_t>();
			AddValue(groupName, itemName, value);
		}
		else if (itItem->is_number_float()) {
			// float型の値
			double value = itItem->get<double>();
			AddValue(groupName, itemName, static_cast<float>(value));
		}
		else if (itItem->is_array() && itItem->size() == 2) {
			// Vector2型の値
			Vector2 value = Vector2{ itItem->at(0), itItem->at(1) };
			AddValue(groupName, itemName, value);
		}
		else if (itItem->is_array() && itItem->size() == 3) {
			// Vector3型の値
			Vector3 value = Vector3{ itItem->at(0), itItem->at(1), itItem->at(2) };
			AddValue(groupName, itemName, value);
		}
		else if (itItem->is_array() && itItem->size() == 4) {
			// Vector4型の値
			Vector4 value = Vector4{ itItem->at(0), itItem->at(1), itItem->at(2), itItem->at(3) };
			AddValue(groupName, itemName, value);
		}
	}
}
//
//void Serializer::SetItems(const std::string& groupName) const {
//	GlobalVariables* const gVariables = GlobalVariables::GetInstance();
//	gVariables->CreateGroups(groupName);
//	for (const auto& item : itemMap_) {
//		gVariables->AddValue<>(
//		    groupName, item.first, item.second.second);
//	}
//}
