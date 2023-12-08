#include "TitleBlock.h"
#include "../../Entity/Component/ModelComp.h"
#include "../../../Engine/DirectBase/Model/ModelManager.h"

void TitleBlock::Init()
{
	// モデルコンポーネントの追加
	object_->AddComponent<ModelComp>();
	// モデルコンポーネントを取得しボーン追加
	object_->GetComponent<ModelComp>()->AddBone("Main", ModelManager::GetInstance()->GetModel("GrassModel"));
}
