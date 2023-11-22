#pragma once
#include <string>
#include "../../../../Utils/Math/Vector3.h"
#include "../../Entity.h"
#include "../ModelComp.h"

// クラスの前方宣言
class GlobalVariables;

/// <summary>
/// プレイヤーボーン
/// </summary>
class PlayerBone
{
private: // サブクラス

	// SRT構造体
	struct InnerTransform {
		Vector3 scale;	   // 拡縮
		Vector3 rotate;	   // 回転
		Vector3 translate; // 位置

		void Initilaize() {
			scale = { 1.0f, 1.0f, 1.0f };
			rotate = { 0.0f, 0.0f, 0.0f };
			translate = { 0.0f, 0.0f, 0.0f };
		}
	};

public: // パブリックなサブクラス

	struct Bone {
		InnerTransform body;   // 体
		InnerTransform eye;	  // 瞳
		InnerTransform ling;	  // 輪っか
		InnerTransform arm_L;  // 左腕
		InnerTransform arm_R;  // 右腕
		InnerTransform foot_L; // 左足
		InnerTransform foot_R; // 右足

		void Initilaize() {
			body.Initilaize();   // 体
			eye.Initilaize();	 // 瞳
			ling.Initilaize();	 // 輪っか
			arm_L.Initilaize();  // 左腕
			arm_R.Initilaize();  // 右腕
			foot_L.Initilaize(); // 左足
			foot_R.Initilaize(); // 右足
		}
	};

public: //　メンバ関数

	/// <summary>
	/// 初期化関数
	/// </summary>
	void Initialize(std::string groupName, std::string boneName);
	/// <summary>
	/// 初期化関数
	/// </summary>
	/// <param name="player">プレイヤー</param>
	void Initialize(Entity* player, std::string groupName, std::string boneName);

public: // アクセッサ等

	/// <summary>
	/// 呼び出した際のBone情報をプレイヤーエンティティに送信
	/// </summary>
	void SetToEntity();

	/// <summary>
	/// グローバル変数クラスに自分自身を登録する関数
	/// </summary>
	/// <param name="gv">グローバル変数クラス実体</param>
	void AddItem(GlobalVariables* gv);

	/// <summary>
	/// グローバル変数クラスに値をセットする関数
	/// </summary>
	/// <param name="gv">グローバル変数クラス実体</param>
	void SetItem(GlobalVariables* gv);
	/// <summary>
	/// グローバル変数クラスの値を適用
	/// </summary>
	/// <param name="gv">グローバル変数クラス実体</param>
	void ApplyItem(GlobalVariables* gv);

	/// <summary>
	/// ImGui表示関数
	/// </summary>
	/// <param name="groupName">グループ名</param>
	void DisplayImGui();

	/// <summary>
	/// ボーン名のゲッター
	/// </summary>
	/// <returns>ボーン名</returns>
	std::string GetBoneName() { return boneName_; }
	/// <summary>
	/// ボーン名セッター
	/// </summary>
	/// <param name="name">ボーン名</param>
	void SetBoneName(std::string name) { boneName_ = name; }

public: // パブリックなメンバ関数

	// ボーン本体
	Bone bone_;

private: // メンバ変数

	// 所属するグループ名
	std::string groupName_;
	// 所属するボーン名
	std::string boneName_;

	// プレイヤー
	Entity* player_ = nullptr;
};

