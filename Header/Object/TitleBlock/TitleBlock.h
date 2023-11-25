#pragma once
#include "../../Entity/Entity.h"

/// <summary>
/// タイトルで表示させるブロック
/// </summary>
class TitleBlock : public IComponent
{
public: // メンバ関数

	// 基底コンポーネントのコンストラクタを使用
	using IComponent::IComponent;
	// デストラクタ
	~TitleBlock() = default;

	/// <summary>
	/// 初期化関数
	/// </summary>
	void Init() override;
};

