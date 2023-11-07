#pragma once
#include <vector>
#include <string>
#include "AnimEasing.h"
#include "PlayerBone.h"

// クラスの前方宣言
class GlobalVariables;

/// <summary>
/// アニメーションキー
/// </summary>
class AnimationKeys
{
public: // パブリックなサブクラス

	//　キーフレーム1つ
	struct Key {
		PlayerBone bone;
		int type;
		float animationTime;

		void Initialize() {
			bone.Initialize("None", "None");
			type = AnimEasing::kLinear;
			animationTime = 0.0f;
		}
	};

public: // メンバ関数

	void Initialize(std::string groupName);

	void AddKey();

	void AddKeyInfo();
	void SetKeyInfo();
	void ApplyKeyInfo();

	/// <summary>
	/// ImGuiの表示
	/// </summary>
	void ShowImGUi();

public: // キーたち

	// キーフレーム数
	int keyCount_;

	// アニメーションキー達
	std::vector<Key> keys_;

private: // メンバ変数

	// グローバル変数クラス
	GlobalVariables* gv_;
	// このキーフレーム達が所属するグループ名
	std::string groupName_;
	
	// コピーされたキー
	Key copiedKey_;

};