#include "AnimationKeys.h"
#include "../../../../Engine/DirectBase/File/GlobalVariables.h"

void AnimationKeys::Initialize(std::string groupName)
{
	// グループ名の取得
	groupName_ = groupName;
	keyCount_ = 0;
	gv_ = GlobalVariables::GetInstance();

	// コピーキーの初期化
	copiedKey_.Initialize();
}

void AnimationKeys::AddKey()
{
	// 追加するキー
	Key tempKey;
	PlayerBone bone;
	// 引数の値を代入する
	tempKey.bone = bone;
	std::string fullBoneName = "key : " + std::to_string((int)keys_.size());
	tempKey.bone.Initialize(groupName_, fullBoneName);
	tempKey.type = AnimEasing::kLinear;
	tempKey.animationTime = 1.0f;

	// キーを追加する
	keys_.push_back(tempKey);

	// キーフレーム数を取得
	keyCount_ = (int)keys_.size();
}

void AnimationKeys::AddKeyInfo()
{
	// キーフレーム数を取得
	keyCount_ = (int)keys_.size();
	gv_->AddValue(groupName_, "KeyCount", keyCount_);
	// 全ての情報をグローバル変数に追加する
	for (int i = 0; i < (int)keys_.size(); i++) {
		keys_[i].bone.AddItem(gv_);
		gv_->AddValue(groupName_, keys_[i].bone.GetBoneName() + "EasingType", keys_[i].type);
		gv_->AddValue(groupName_, keys_[i].bone.GetBoneName() + "AnimationTime", keys_[i].animationTime);
	}
}

void AnimationKeys::SetKeyInfo()
{
	// キーフレーム数を取得
	keyCount_ = (int)keys_.size();
	gv_->SetValue(groupName_, "KeyCount", keyCount_);
	// 全ての情報をグローバル変数に追加する
	for (int i = 0; i < (int)keys_.size(); i++) {
		keys_[i].bone.SetItem(gv_);
		gv_->SetValue(groupName_, keys_[i].bone.GetBoneName() + "EasingType", keys_[i].type);
		gv_->SetValue(groupName_, keys_[i].bone.GetBoneName() + "AnimationTime", keys_[i].animationTime);
	}
}

void AnimationKeys::ApplyKeyInfo()
{
	// キーフレーム数をグローバル変数クラスを取得
	int count = gv_->Get<int>(groupName_, "KeyCount");
	// キー配列をクリア
	keys_.clear();
	// 全ての情報をグローバル変数に追加する
	for (int i = 0; i < count; i++) {
		// 追加するキー
		Key tempKey;
		PlayerBone bone;
		// 引数の値を代入する
		tempKey.bone = bone;
		std::string fullBoneName = "key : " + std::to_string((int)keys_.size());
		tempKey.bone.Initialize(groupName_, fullBoneName);
		tempKey.bone.ApplyItem(gv_);
		tempKey.type = gv_->Get<int>(groupName_, fullBoneName + "EasingType");
		tempKey.animationTime = gv_->Get<float>(groupName_, fullBoneName + "AnimationTime");

		// キーを追加する
		keys_.push_back(tempKey);
		// キーフレーム数を取得
		keyCount_ = (int)keys_.size();
	}
}

void AnimationKeys::ShowImGUi()
{
	ImGui::Begin("AnimManager", nullptr, ImGuiWindowFlags_MenuBar);
	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu(groupName_.c_str())) {
			ImGui::DragInt("KeyCount", &keyCount_, 0.0f, 0, 100);
			if (ImGui::TreeNode("AddMenu")) {
				if (ImGui::Button("AddKey"))
					AddKey();

				if (!ImGuiKeepAdding_) {
					if (ImGui::Button("AddKeyInfo")) {
						AddKeyInfo();
						SetKeyInfo();
					}
				}
				else {
					AddKeyInfo();
					SetKeyInfo();
				}

				if (ImGui::Button("ApplyKeyInfo")) {
					ApplyKeyInfo();
				}

				if (ImGui::Button("SaveFile")) {
					gv_->SaveFile(groupName_);
				}

				if (!ImGuiKeepAdding_) {
					if (ImGui::Button("Enable KeepAdd"))
						ImGuiKeepAdding_ = true;
				}
				else {
					if (ImGui::Button("Disable KeepAdd"))
						ImGuiKeepAdding_ = false;
				}

				ImGui::TreePop();
			}
			// 全ての情報を表示する
			for (int i = 0; i < (int)keys_.size(); i++) {
				keys_[i].bone.DisplayImGui();
				std::string name = "EasingType" + std::to_string(i);
				ImGui::SliderInt(name.c_str(), &keys_[i].type, 0, 3);
				name = "AnimationTime" + std::to_string(i);
				ImGui::DragFloat(name.c_str(), &keys_[i].animationTime, 0.05f, 0.05f, 10.0f);
				name = "Delete : " + std::to_string(i);
				if (ImGui::Button(name.c_str())) {
					keys_.erase(keys_.begin() + i);
					keyCount_ = (int)keys_.size();
					for (int j = 0; j < keyCount_; j++) {
						std::string fullBoneName = "key : " + std::to_string(j);
						keys_[j].bone.SetBoneName(fullBoneName);
					}
				}

				name = "CopyKey : " + std::to_string(i);
				if (ImGui::Button(name.c_str()))
					copiedKey_ = keys_[i];

				if (copiedKey_.bone.GetBoneName() != "None") {
					name = "PasteKey : " + std::to_string(i);
					if (ImGui::Button(name.c_str())) {
						copiedKey_.bone.SetBoneName(keys_[i].bone.GetBoneName());
						keys_[i] = copiedKey_;
					}
				}
			}
			ImGui::EndMenu();
		}
		
	}
	ImGui::EndMenuBar();
	ImGui::End();
}
