#pragma once
#include "../../Engine/DirectBase/2D/Sprite.h"
#include "../../Engine/Utils/SoLib/SoLib.h"

class ButtonPickUp;

class ButtonUI {
public:

	friend ButtonPickUp;

	Transform2D normalTrans_;
	Transform2D pickUpTrans_;

	//private:
		// ボタンの画像
	std::unique_ptr<Sprite> sprite_;
};

class ButtonPickUp {
public:

	static constexpr uint32_t kDropDownCount_ = 2u;

public:

	bool ImGuiWidget();

	void Pickup(ButtonUI *target);

	void Update(const float deltaTime);

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

	void Init();

	void Start();

	void InputFunc();

	void Update(const float deltaTime);

	void Draw() const;

private:

	// ボタンのカーソル
	int32_t target_;

	// ボタンの数
	int32_t targetCount_;

	// 背景
	std::unique_ptr<Sprite> backGround_;

	// ボタンのUI
	std::vector<std::unique_ptr<ButtonUI>> button_;

	// 描画のタイマー
	SoLib::DeltaTimer timer_;

};