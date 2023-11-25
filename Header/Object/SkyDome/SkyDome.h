#pragma once
#include "../../Entity/Entity.h"

class SkyDome : public IComponent
{
public: // メンバ関数

	// 基底コンポーネントのコンストラクタを使用
	using IComponent::IComponent;
	// デストラクタ
	~SkyDome() = default;

	/// <summary>
	/// 初期化関数
	/// </summary>
	void Init() override;
};

