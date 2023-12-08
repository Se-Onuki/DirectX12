#include "TitleManager.h"
#include "../../Object/SkyDome/SkyDome.h"
#include "../../Object/TitleBlock/TitleBlock.h"
#include "../../Entity/Component/PlayerAnimComp.h"

void TitleManager::Initialize()
{
	// インスタンス取得
	cameraManager_ = CameraManager::GetInstance();
	input_ = Input::GetInstance();

	cameraManager_->AddCamera("BlurCamera");
	cameraManager_->cameraList_->GetCamera("BlurCamera")->translation_ = cameraManager_->GetUseCamera()->translation_;
	cameraManager_->cameraList_->GetCamera("BlurCamera")->rotation_ = cameraManager_->GetUseCamera()->rotation_;
	cameraManager_->SetUseCamera(cameraManager_->cameraList_->GetCamera("BlurCamera"));
	cameraManager_->AddCamera("BlurCamera2");
	cameraManager_->cameraList_->GetCamera("BlurCamera2")->translation_ = cameraManager_->GetUseCamera()->translation_;
	cameraManager_->cameraList_->GetCamera("BlurCamera2")->rotation_ = cameraManager_->GetUseCamera()->rotation_;
	cameraManager_->cameraList_->GetCamera("BlurCamera2")->translation_.y -= 0.15f;
	cameraManager_->cameraList_->GetCamera("BlurCamera")->rotation_.x -= 0.01f;

	// エンティティの生成
	skyDome_ = std::make_unique<Entity>();
	skyDome_->Init();
	playerModel_ = std::make_unique<Entity>();
	playerModel_->Init();
	blockModel_ = std::make_unique<Entity>();
	blockModel_->Init();

	// モデル取得
	skyDome_->AddComponent<SkyDome>();
	playerModel_->AddComponent<PlayerAnimComp>();
	blockModel_->AddComponent<TitleBlock>();

	// プレイヤーモデルのアニメーションを初期化
	playerModel_->GetComponent<PlayerAnimComp>()->GetAnimManager()->SetNextAnimation(PlayerBehavior::kTitle, true);
	playerModel_->transform_.translate = { -4.0f, -1.25f, -92.0f };
	playerModel_->transform_.rotate = { 0.0f, -1.0f, -0.0f };
	blockModel_->transform_.translate = { -4.0f, -2.8f, -92.0f };

	titleUI_.sprite_.reset(Sprite::Create(TextureManager::Load("UI/TD2_3week_2/Title/Title.png")));
	titleUI_.position_ = { 1280.0f / 2.0f, 300.0f };
	titleUI_.scale_ = { 1280.0f, 256.0f };
	titleUI_.anchorPoint_ = { 0.5f, 0.5f };

	circleUI_.sprite_.reset(Sprite::Create(TextureManager::Load("UI/TD2_3week_2/Title/TitleCircle.png")));
	circleUI_.position_ = { 847.0f, 224.0f };
	circleUI_.scale_ = { 64.0f, 64.0f };
	circleUI_.anchorPoint_ = { 0.5f, 0.5f };

	buttonUI_.sprite_.reset(Sprite::Create(TextureManager::Load("UI/TD2_3week_2/Title/Start_Key.png")));
	buttonUI_.position_ = { 1280.0f / 2.0f, 600.0f };
	buttonUI_.scale_ = { 512.0f, 128.0f };
	buttonUI_.anchorPoint_ = { 0.5f, 0.5f };

	rotateImage_ = 0.0f ;

	startTitlePos_ = { 1280.0f / 2.0f, 300.0f };
	endTitlePos_ = { 1280.0f / 2.0f, 315.0f };
	startCirclePos_ = { 847.0f, 224.0f };
	endCirclePos_ = { 847.0f, 244.0f };

	timer_.Start(2.0f);
}

void TitleManager::Update([[maybe_unused]]float deltaTime)
{

	// 天球を回転させる
	skyDome_->transform_.rotate.y += 0.001f;
	skyDome_->transform_.rotate.y = Angle::Mod(skyDome_->transform_.rotate.y);

	rotateImage_ += 0.0075f;
	rotateImage_ = Angle::Mod(rotateImage_);

	circleUI_.sprite_->SetRotate(rotateImage_);

	skyDome_->Update(deltaTime);
	playerModel_->Update(deltaTime);
	blockModel_->Update(deltaTime);

	if (!isReturn_) {
		if (!cameraManager_->GetCameraAnimManager()->GetIsPlaying()) {
			cameraManager_->GetCameraAnimManager()->Play(cameraManager_->cameraList_->GetCamera("BlurCamera2"), 2.0f, SoLib::easeInOutQuad, false);
			isReturn_ = true;
		}
	}
	else{
		if (!cameraManager_->GetCameraAnimManager()->GetIsPlaying()) {
			cameraManager_->GetCameraAnimManager()->Play(cameraManager_->cameraList_->GetCamera("BlurCamera"), 2.0f, SoLib::easeInOutQuad, false);
			isReturn_ = false;
		}
	}

	if (not timer_.IsFinish()) {
		titleUI_.position_ = SoLib::Lerp(startTitlePos_, endTitlePos_, SoLib::easeInOutQuad(timer_.GetProgress()));
		circleUI_.position_ = SoLib::Lerp(startCirclePos_, endCirclePos_, SoLib::easeInOutQuad(timer_.GetProgress()));
	}
	else {
		titleUI_.position_ = endTitlePos_;
		endTitlePos_ = startTitlePos_;
		startTitlePos_ = titleUI_.position_;

		circleUI_.position_ = endCirclePos_;
		endCirclePos_ = startCirclePos_;
		startCirclePos_ = circleUI_.position_;

		timer_.Start(2.0f);
	}

	timer_.Update(deltaTime);

	if(input_->GetInputType() == Input::InputType::kPad){
		buttonUI_.sprite_->SetTextureHaundle(TextureManager::Load("UI/TD2_3week_2/Title/Start_CR.png"));
	}
	else {
		buttonUI_.sprite_->SetTextureHaundle(TextureManager::Load("UI/TD2_3week_2/Title/Start_Key.png"));
	}

}

void TitleManager::Draw(const Camera3D& camera)
{
	// 天球の描画
	skyDome_->Draw(camera);
	playerModel_->Draw(camera);
	blockModel_->Draw(camera);
}

void TitleManager::SpriteDraw()
{
	titleUI_.Draw();
	circleUI_.Draw();
	buttonUI_.Draw();
}
