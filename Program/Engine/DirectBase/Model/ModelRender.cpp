#include "ModelRender.h"

void SolEngine::ModelRender::Init()
{
}

void SolEngine::ModelRender::clear()
{
	for (auto &map : modelsBuffer_) {
		map.clear();
	}

}

void SolEngine::ModelRender::AddData(ModelManager::Handle model, const Transform &transform)
{
	for (const MeshManager::Handle mesh : model->meshHandleList_) {
		const MaterialManager::Handle material = mesh->materialhandle_;
		auto &buff = modelsBuffer_[static_cast<uint32_t>(material->blendMode_)];
		buff[&transform].push_back(mesh);
	}

}
