#include "ModelRender.h"

void SolEngine::ModelRender::Init()
{
	dxCommon_ = DirectXCommon::GetInstance();
	commandList_ = dxCommon_->GetCommandList();

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

void SolEngine::ModelRender::Sort()
{
	for (auto &buffs : modelsBuffer_) {
		for (auto [transform, meshs] : buffs) {
			std::sort(meshs.begin(), meshs.end());
		}
	}
}

void SolEngine::ModelRender::Draw(const Camera3D &camera)
{
	assert(Model::GetPipelineType() == Model::PipelineType::kModel && "設定されたシグネチャがkModelではありません");

	commandList_->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kViewProjection, camera.constData_.GetGPUVirtualAddress());

	{
		for (auto [transform, meshs] : modelsBuffer_[static_cast<uint32_t>(Model::BlendMode::kNone)]) {
			// std::sort(meshs.begin(), meshs.end());

			commandList_->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kWorldTransform, transform->GetGPUVirtualAddress());
			for (auto &mesh : meshs) {
				mesh->Draw(commandList_);
			}

		}
	}

}
