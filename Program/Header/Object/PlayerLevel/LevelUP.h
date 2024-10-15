#pragma once
#include "../../Engine/DirectBase/2D/Sprite.h"
#include "../../Engine/Utils/SoLib/SoLib.h"

class ButtonPickUp;

class ButtonUI {
public:

	static std::unique_ptr<ButtonUI> Generate();

	friend ButtonPickUp;

	// 実行する処理
	std::function<void(void)> execute_;

	SoLib::Color::RGB4 normalColor_;
	SoLib::Color::RGB4 pickUpColor_;

	// ボタンの画像
	std::unique_ptr<Sprite> sprite_;

	void Draw() const;
};

class ButtonUIGroup
{
public:
	ButtonUIGroup() = default;
	~ButtonUIGroup() = default;

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
	SoLib::EaseFunc pickUpEase_;
	// ピックアップ時のタイマー
	SoLib::Time::DeltaTimer pickUpTimer_{ 0.5f };

	// 解除時のイージング
	SoLib::EaseFunc dropDownEase_;
	// 解除時に必要な時間
	float dropDownTime_ = 0.5f;

	// 指定するターゲット
	ButtonUI *pickUpTarget_;
	// 解除するターゲット
	std::array<std::pair<ButtonUI *, float>, kDropDownCount_> dropDownTarget_;

};

class LevelUP
{
public:
	LevelUP() = default;
	~LevelUP() = default;

	void Init(int32_t count = 3);

	void SetWindow(Vector2 center, Vector2 scale, float distance);

	void Start(int32_t target = 1);

	void InputFunc();

	void Update(const float deltaTime);

	void Draw() const;

	void Target(int32_t target);

private:

	// 画面の中心位置
	Vector2 windowCenter_;
	// ボタンの大きさ
	Vector2 buttonScale_;
	// ボタンの隙間
	float distance_;

	// ボタンのカーソル
	int32_t target_;

	// ボタンの数
	int32_t targetCount_;

	// 背景
	std::unique_ptr<Sprite> backGround_;

	// ボタンのUI
	std::vector<std::unique_ptr<ButtonUI>> button_;

	// ボタンUIの集合
	ButtonUIGroup buttonUIGroup_;

	// ボタンのアニメーション
	ButtonPickUp buttonPicker_;

	// アニメーションタイマー
	SoLib::DeltaTimer timer_;

};