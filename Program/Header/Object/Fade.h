/// @file Fade.h
/// @brief フェードクラス
/// @author ONUKI seiya
#pragma once
#include <stdint.h>
#include <memory>

#include "../../Engine/Utils/SoLib/SoLib_Timer.h"
#include "../../Engine/Utils/SoLib/SoLib_Easing.h"

#include "../../Engine/Utils/Math/Vector2.h"
#include "../../Engine/Utils/Math/Vector4.h"
#include "../../Engine/Utils/Math/Math.hpp"
#include "../../Engine/DirectBase/2D/Sprite.h"

#include "../../Engine/Utils/Graphics/Color.h"

class Fade {
	Fade() = default;
	Fade(const Fade &) = delete;
	Fade &operator=(const Fade &) = delete;
	~Fade() = default;
public:

	static Fade *const GetInstance() {
		static Fade instance{};
		return &instance;
	}

	/// @brief 初期化
	static void StaticInit();
	/// @brief 状態の設定
	void SetState(const Vector2 pos, const SoLib::Color::RGB4 &alpha);

	/// @brief フェードの開始
	void Start(const Vector2 targetPos, const SoLib::Color::RGB4 &targetAlpha, float goalTime);
	void Update(float deltaTime);
	void Draw();

	/// @brief イージング関数の設定
	void SetEaseFunc(float (*easeFunc)(float)) { easeFunc_ = easeFunc; }

	/// @brief スプライトの取得
	Sprite *const GetSprite() { return sprite_.get(); }
	/// @brief タイマーの取得
	const SoLib::DeltaTimer *const GetTimer() { return timer_.get(); }

	static const SoLib::Color::RGB4 kFadeColor_;

private:

	Vector2 targetPos_{};	// 移動後の座標
	Vector2 originalPos_{};	// 移動前の座標

	SoLib::Color::RGB4 targetColor_{};	// 変更後の色
	SoLib::Color::RGB4 originalColor_{};	// 変更前の色

	/// @brief イージング関数の関数ポインタ
	float (*easeFunc_)(float) = SoLib::easeLinear;

	std::unique_ptr<SoLib::DeltaTimer> timer_ = nullptr;
	std::unique_ptr<Sprite> sprite_ = nullptr;
};