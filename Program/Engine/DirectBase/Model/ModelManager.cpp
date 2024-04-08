#include "ModelManager.h"

#include <memory>
#include <array>
#include "../Base/TextureManager.h"

MeshManager *const MeshManager::Key::pMeshManager_ = MeshManager::GetInstance();

void ModelManager::CreateDefaultModel() {
	AddModel("Plane", Model::CreatePlane());
}

MeshManager::Key MeshManager::AddMesh(std::unique_ptr<Mesh> mesh)
{
	meshArray_.push_back(std::move(mesh));
	return static_cast<uint32_t>(meshArray_.size() - 1);
}
