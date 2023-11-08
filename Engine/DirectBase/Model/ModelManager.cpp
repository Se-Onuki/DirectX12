#include "ModelManager.h"

#include <memory>
#include <array>
#include "../Base/TextureManager.h"

void ModelManager::CreateDefaultModel() {
	AddModel("Plane", Model::CreatePlane());
}
