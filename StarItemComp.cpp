#include "StarItemComp.h"
#include "Header/Entity/Component/ModelComp.h"
#include "Engine/DirectBase/Model/ModelManager.h"

void StarItemComp::Init() {
	auto *const modelComp = object_->AddComponent<ModelComp>();

	modelComp->AddBone("Body", ModelManager::GetInstance()->GetModel("StarItem"));

}

void StarItemComp::Update() {
}
