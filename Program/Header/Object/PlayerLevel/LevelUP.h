/// @file LevelUP.h
/// @brief レベルアップのUI
/// @author ONUKI seiya
#pragma once
#include "../../Engine/DirectBase/2D/Sprite.h"
#include "../../Engine/Utils/SoLib/SoLib.h"

class ButtonPickUp;

class ButtonUI {
public:

	/// @brief 生成
	static std::unique_ptr<ButtonUI> Generate();

	void Init(const uint32_t texture, const std::function<void(void)> &func);

	friend ButtonPickUp;

	// 実行する処理
	std::function<void(void)> execute_;

	// 通常時
	SoLib::Color::RGB4 normalColor_ = 0xFFFFFFFF;
	// 選択時
	SoLib::Color::RGB4 pickUpColor_ = 0x00FFFFFF;

	// ボタンの画像
	std::unique_ptr<Sprite> sprite_ = nullptr;

	void Draw() const;
};

class ButtonUIGroup
{
public:
	ButtonUIGroup() = default;
	~ButtonUIGroup() = default;

	/// @brief ボタンの状態を設定
	void SetStatus(const std::function<void(ButtonUI *, size_t index)> &statusSetter) const;


	// ボタンの集合
	std::vector<ButtonUI *> buttonUIList_;

};

class ButtonPickUp {
public:

	static constexpr uint32_t kDropDownCount_ = 2u;

public:

	void Reset();

	/// @brief 選択中か
	/// @return ボタンが登録されてたら true
	bool IsPick() const { return pickUpTarget_ != nullptr; }

	bool ImGuiWidget();

	void Pickup(ButtonUI *target);

	/// @brief 更新処理
	/// @param deltaTime 時間差分
	void Update(const float deltaTime);

	/// @brief ボタンに指定された関数の実行
	void Execute() const;

private:
	// ピックアップ時のイージング
	SoLib::EaseFunc pickUpEase_{};
	// ピックアップ時のタイマー
	SoLib::Time::DeltaTimer pickUpTimer_{ 0.5f };

	// 解除時のイージング
	SoLib::EaseFunc dropDownEase_{};
	// 解除時に必要な時間
	float dropDownTime_ = 0.5f;

	// 指定するターゲット
	ButtonUI *pickUpTarget_ = nullptr;
	// 解除するターゲット
	std::array<std::pair<ButtonUI *, float>, kDropDownCount_> dropDownTarget_{};

};

class LevelUP
{
public:
	LevelUP() = default;
	~LevelUP() = default;

	void Init(int32_t count = 4);

	void SetWindow(Vector2 center, Vector2 scale, float distance);

	/// @brief メニューオープン
	/// @param [in] target 指定する番号
	void Open(int32_t target = 1);

	/// @brief メニュー非表示
	void Close();

	void InputFunc();

	void Update(const float deltaTime);

	void Draw() const;

	void Target(int32_t target);

	void RandomSet();

	/// @brief メニューが開いているか
	/// @return メニューが開いているならTrue
	bool IsActive() const { return isOpen_ or timer_.IsActive(); }

	/// @brief メニューが開いているときの係数
	/// @return 0が閉じていて､1が開いている
	float OpenProgress() const {
		float t = timer_.GetProgress();
		if (t == 0.f) { t = 1.f; }
		return isOpen_ ? t : 1 - t;
	}

	ButtonUI *const GetButtonUI(uint32_t index) { return index >= button_.size() ? nullptr : button_[index].get(); }

	void push_back(std::unique_ptr<ButtonUI> &&button);

private:

	// 画面の中心位置
	Vector2 windowCenter_{};
	// ボタンの大きさ
	Vector2 buttonScale_{};
	// ボタンの隙間
	float distance_{};

	// ボタンのカーソル
	int32_t target_{};

	// ボタンの数
	int32_t targetCount_{};

	// 背景
	std::unique_ptr<Sprite> backGround_ = nullptr;

	// ボタンのUI
	std::vector<std::unique_ptr<ButtonUI>> button_;

	// ボタンUIの集合
	ButtonUIGroup buttonUIGroup_{};

	// ボタンのアニメーション
	ButtonPickUp buttonPicker_{};

	// アニメーションタイマー
	SoLib::DeltaTimer timer_{};

	// 開くためのフラグ
	bool isOpen_ = false;

	// 選択しているか
	bool isSelect_ = false;

};