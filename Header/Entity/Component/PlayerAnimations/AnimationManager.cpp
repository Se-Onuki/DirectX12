#include "AnimationManager.h"
#include "BaseAnimation.h"

void AnimationManager::Initialize()
{

	// アニメーション数値クラスを生成
	animParameters_ = std::make_unique<AnimationParameters>(); // 生成
	animParameters_->Initialize();							   // 初期化

	// 現在アニメーションをアイドルに設定
	currentAnimation_ = std::make_unique<BaseAnimation>();	 // インスタンス生成
	currentAnimation_->SetEntity(entity_);		 // アニメーションさせるエンティティをセット
	currentAnimation_->Initialize("Idle", true); // 初期化
}

void AnimationManager::Update()
{
	// FPSカウンターの表示
	ImGui::Begin("Control panel");
	ImGui::Text("Frame rate: %6.2f fps", ImGui::GetIO().Framerate);
	ImGui::End();

	// 更新
	animParameters_->Update();

	const float deltaTime = std::clamp(ImGui::GetIO().DeltaTime, 0.f, 0.1f);

	// 次のアニメーションがセットされている場合
	if (nextAnimation_ != nullptr) {

		currentAnimation_ = std::move(nextAnimation_);  // 現在アニメーションに次のアニメーションをセット

	}
	// 現在アニメーションを更新
	currentAnimation_->Update(deltaTime);

#ifdef _DEBUG
	ImGui::Begin("AnimManager");
	ImGui::RadioButton("PlayIdle", &imGuiNextbehavior_, kIdle);
	ImGui::RadioButton("PlayMove", &imGuiNextbehavior_, kMove);
	ImGui::RadioButton("PlayJumpStart", &imGuiNextbehavior_, kJumpStart);
	ImGui::RadioButton("PlayHovering", &imGuiNextbehavior_, kHovering);
	ImGui::RadioButton("PlayLand", &imGuiNextbehavior_, kLand);
	ImGui::RadioButton("PlayRotateStart", &imGuiNextbehavior_, kRotateStart);
	ImGui::RadioButton("PlayRotating", &imGuiNextbehavior_, kRotating);
	ImGui::RadioButton("PlayRotateEnd", &imGuiNextbehavior_, kRotateEnd);

	int key = currentAnimation_->GetPlayKey();
	ImGui::DragInt("NowPlayKey", &key, 0.05f);

	ImGui::Checkbox("isLoop", &imGuiIsLoop_);

	ImGui::DragFloat("transitionTime", &imGuiTransitionTime_, 0.005f, 0.0f, 5.0f);
	if (ImGui::TreeNode("TransitionType")) {
		ImGui::RadioButton("Linear", &imGuiTransitonType_, AnimEasing::kLinear);
		ImGui::RadioButton("EaseIn", &imGuiTransitonType_, AnimEasing::kEaseIn);
		ImGui::RadioButton("EaseOut", &imGuiTransitonType_, AnimEasing::kEaseOut);
		ImGui::RadioButton("EaseInOut", &imGuiTransitonType_, AnimEasing::KEaseInOut);

		ImGui::TreePop();
	}

	float progress = currentAnimation_->GetAnimationProgress();
	ImGui::DragFloat("AnimationProgress", &progress, 0.01f);

	// アニメーション再生
	if (ImGui::Button("PlayAnim")) {
		currentAnimation_->SetIsEnd(true);
		SetNextAnimation((PlayerBehavior)imGuiNextbehavior_, imGuiIsLoop_, (AnimEasing::EasingType)imGuiTransitonType_, imGuiTransitionTime_);
	}

	if (!currentAnimation_->GetIsEnd()) {
		if (currentAnimation_->GetIsPlay()) {
			if (ImGui::Button("StopAnim"))
				currentAnimation_->SetIsPlay(false);
		}
		else {
			if (ImGui::Button("ReplayAnim"))
				currentAnimation_->SetIsPlay(true);
		}
	}

	ImGui::End();
#endif // _DEBUG
}

void AnimationManager::SetNextAnimation(PlayerBehavior next, bool isLoop, AnimEasing::EasingType type, float transitionTime)
{
	nextAnimation_ = std::make_unique<BaseAnimation>();
	nextAnimation_->SetEntity(entity_);

	// 次のアニメーションを設定
	switch (next) {
	case PlayerBehavior::kIdle:
		nextAnimation_->Initialize("Idle", isLoop, type, transitionTime);
		break;
	case PlayerBehavior::kMove:
		nextAnimation_->Initialize("Move", isLoop, type, transitionTime);
		break;
	case PlayerBehavior::kJumpStart:
		nextAnimation_->Initialize("StartJump", isLoop, type, transitionTime);
		break;
	case PlayerBehavior::kHovering:
		nextAnimation_->Initialize("Hovering", isLoop, type, transitionTime);
		break;
	case PlayerBehavior::kLand:
		nextAnimation_->Initialize("Land", isLoop, type, transitionTime);
		break;
	case PlayerBehavior::kRotateStart:
		nextAnimation_->Initialize("RotateStart", isLoop, type, transitionTime);
		break;
	case PlayerBehavior::kRotating:
		nextAnimation_->Initialize("Rotating", isLoop, type, transitionTime);
		break;
	case PlayerBehavior::kRotateEnd:
		nextAnimation_->Initialize("RotateEnd", isLoop, type, transitionTime);
		break;
	}

	// ビヘイビアをセット
	nextAnimation_->SetBehavior(next);
}
