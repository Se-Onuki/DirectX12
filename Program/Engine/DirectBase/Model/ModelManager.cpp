#include "ModelManager.h"

#include <memory>
#include <array>
#include "../Base/TextureManager.h"

MeshManager *const MeshManager::Key::pMeshManager_ = MeshManager::GetInstance();

void ModelManager::CreateDefaultModel() {
	AddModel("Plane", Model::CreatePlane());
}
