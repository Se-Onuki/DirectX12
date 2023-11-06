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
	currentAnimation_ = new BaseAnimation();	 // インスタンス生成
	currentAnimation_->SetEntity(entity_);		 // アニメーションさせるエンティティをセット
	currentAnimation_->Initialize("Idle", true); // 初期化
}

void AnimationManager::Update()
{
	// 更新
	animParameters_->Update();

	const float deltaTime = std::clamp(ImGui::GetIO().DeltaTime, 0.f, 0.1f);

	// 次のアニメーションがセットされている場合
	if (nextAnimation_ != nullptr) {
		// 現在アニメーションが終了している場合
		if (currentAnimation_->GetIsEnd()) {
			currentAnimation_ = nextAnimation_;  // 現在アニメーションに次のアニメーションをセット
			nextAnimation_ = nullptr;			 // アニメーションを破棄
		}
	}
	// 現在アニメーションを更新
	currentAnimation_->Update(deltaTime);

#ifdef _DEBUG
	ImGui::Begin("AnimManager");
	ImGui::RadioButton("PlayIdle", &imGuiNextbehavior_, kIdle);
	//ImGui::RadioButton("PlayMove", &imGuiNextbehavior_, kMove);

	ImGui::Checkbox("isLoop", &imGuiIsLoop_);

	ImGui::DragFloat("transitionTime", &imGuiTransitionTime_, 0.05f, 0.0f, 5.0f);

	// アニメーション再生
	if (ImGui::Button("PlayAnim")) {
		currentAnimation_->SetIsEnd(true);
		SetNextAnimation((Behavior)imGuiNextbehavior_, imGuiIsLoop_, imGuiTransitionTime_);
	}
	ImGui::End();
#endif // _DEBUG
}

void AnimationManager::SetNextAnimation(Behavior next, bool isLoop, float transitionTime)
{
	nextAnimation_ = new BaseAnimation();
	nextAnimation_->SetEntity(entity_);

	// 次のアニメーションを設定
	switch (next)
	{
	case AnimationManager::kIdle:
		nextAnimation_->Initialize("Idle", isLoop, transitionTime);
		break;
	case AnimationManager::kMove:
		break;
	case AnimationManager::kJump:
		break;
	}
}
