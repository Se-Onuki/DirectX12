#include "GoalAnimComp.h"
#include "../ModelComp.h"
#include "../../../../Engine/DirectBase/Model/ModelManager.h"

void GoalAnimComp::Init()
{
	// モデルコンポーネントの追加
	object_->AddComponent<ModelComp>();
	// ゴール
	object_->GetComponent<ModelComp>()->AddBone("Base", ModelManager::GetInstance()->GetModel("Flag"));
}

void GoalAnimComp::Update()
{
	
}
