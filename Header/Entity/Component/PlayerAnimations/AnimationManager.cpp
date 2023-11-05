#include "AnimationManager.h"

void AnimationManager::Initialize()
{

	// アニメーション数値クラスを生成
	animParameters_ = std::make_unique<AnimationParameters>(); // 生成
	animParameters_->Initialize();							   // 初期化

	// 調整追加
	animParameters_->AddItem();
	animParameters_->ApplyItem();

	// 現在アニメーションをアイドルに設定
	currentAnimation_ = new Idle();		   // インスタンス生成
	currentAnimation_->SetEntity(entity_); // アニメーションさせるエンティティをセット
	currentAnimation_->Initialize();	   // 初期化
}

void AnimationManager::Update()
{

	// 更新
	animParameters_->Update();

	const float deltaTime = std::clamp(ImGui::GetIO().DeltaTime, 0.f, 0.1f);

	// 次のアニメーションがセットされている場合
	if (nextAnimation_ != nullptr) {
		// 現在アニメーションが終了している場合
		if (!currentAnimation_->GetIsEnd()) {
			currentAnimation_ = std::move(nextAnimation_); // 現在アニメーションに次のアニメーションをセット
			currentAnimation_->SetEntity(entity_);		   // アニメーションさせるエンティティをセット
			currentAnimation_->Initialize();			   // 初期化
		}
	}
	// 現在アニメーションを更新
	currentAnimation_->Update(deltaTime);

#ifdef _DEBUG
	animParameters_->SetItem();
	// 適用
	animParameters_->ApplyItem();
#endif // _DEBUG


}
