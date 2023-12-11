#include "SkyDome.h"
#include "../../Entity/Component/ModelComp.h"
#include "../../../Engine/DirectBase/Model/ModelManager.h"

void SkyDome::Init()
{
	// モデルコンポーネントの追加
	object_->AddComponent<ModelComp>();
	// モデルコンポーネントを取得しボーン追加
	object_->GetComponent<ModelComp>()->AddBone("Main", ModelManager::GetInstance()->GetModel("SkyDome"));
}
