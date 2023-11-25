#include "TitleManager.h"
#include "../../Object/SkyDome/SkyDome.h"
#include "../../Entity/Component/PlayerAnimComp.h"

void TitleManager::Initialize()
{
	// エンティティの生成
	skyDome_ = std::make_unique<Entity>();
	skyDome_->Init();

	playerModel_ = std::make_unique<Entity>();
	playerModel_->Init();

	// モデル取得
	skyDome_->AddComponent<SkyDome>();
	playerModel_->AddComponent<PlayerAnimComp>();

	// プレイヤーモデルのアニメーションを初期化
	playerModel_->GetComponent<PlayerAnimComp>()->GetAnimManager()->SetNextAnimation(PlayerBehavior::kTitle, true);
}

void TitleManager::Update([[maybe_unused]]float deltaTime)
{
	skyDome_->Update(deltaTime);
	playerModel_->Update(deltaTime);
}

void TitleManager::Draw(const Camera3D& camera)
{
	// 天球の描画
	skyDome_->Draw(camera);
	playerModel_->Draw(camera);
}
