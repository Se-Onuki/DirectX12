#include "AnimationManager.h"

void AnimationManager::Initialize()
{

	// アニメーション数値クラスを生成
	animParameters_ = std::make_unique<AnimationParameters>(); // 生成
	animParameters_->Initialize();							   // 初期化

	// 調整追加
	animParameters_->AddItem();
	animParameters_->ApplyItem();

	// 遷移秒数リセット
	transitionTime_ = 0.0f;

	// 現在アニメーションをアイドルに設定
	//currentAnimation_ = new Idle();		   // インスタンス生成
	//currentAnimation_->SetEntity(entity_); // アニメーションさせるエンティティをセット
	//currentAnimation_->Initialize();	   // 初期化
}

void AnimationManager::Update()
{
	// 更新
	animParameters_->Update();

	//const float deltaTime = std::clamp(ImGui::GetIO().DeltaTime, 0.f, 0.1f);

	//// 次のアニメーションがセットされている場合
	//if (nextAnimation_ != nullptr) {
	//	// 現在アニメーションが終了している場合
	//	if (currentAnimation_->GetIsEnd()) {
	//		currentAnimation_ = nextAnimation_;  // 現在アニメーションに次のアニメーションをセット
	//		nextAnimation_ = nullptr;			 // アニメーションを破棄
	//		currentAnimation_->SetEntity(entity_);		    // アニメーションさせるエンティティをセット
	//		currentAnimation_->Initialize(transitionTime_);	// 初期化
	//	}
	//}
	//// 現在アニメーションを更新
	//currentAnimation_->Update(deltaTime);

#ifdef _DEBUG

	//ImGui::Begin("AnimManager");
	//ImGui::RadioButton("PlayIdle", &imGuiNextbehavior_, kIdle);
	//ImGui::RadioButton("PlayMove", &imGuiNextbehavior_, kMove);

	//ImGui::DragFloat("transitionTime", &imGuiTransitionTime_, 0.05f, 0.0f, 5.0f);

	//// アニメーション再生
	//if (ImGui::Button("PlayAnim")) {
	//	currentAnimation_->SetIsEnd(true);
	//	SetNextAnimation((Behavior)imGuiNextbehavior_, imGuiTransitionTime_);
	//}
	//ImGui::End();

	//// 値をセット
	//animParameters_->SetItem();
	//// 適用
	//animParameters_->ApplyItem();
#endif // _DEBUG
}

void AnimationManager::SetNextAnimation(Behavior next, float transitionTime)
{
	// 遷移時間を取得
	transitionTime_ = transitionTime;

	// 次のアニメーションを設定
	switch (next)
	{
	case AnimationManager::kIdle:
		nextAnimation_ = new Idle();
		break;
	case AnimationManager::kMove:
		nextAnimation_ = new Move();
		break;
	case AnimationManager::kJump:
		break;
	}
}
