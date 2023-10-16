#include "ModelManager.h"

#include <memory>
#include <array>
#include "../Base/TextureManager.h"

void ModelManager::CreateDefaultModel() {
	CreatePlane();
}

void ModelManager::CreatePlane() {
	std::unique_ptr<Model> newModel = std::make_unique<Model>();

	newModel->meshList_.push_back(std::make_unique<Mesh>());
	auto &mesh = *newModel->meshList_.begin();

	auto &vertexArray = mesh->vertexBuffer_;
	vertexArray.SetVertexData(std::array{ Mesh::VertexData{},Mesh::VertexData{},Mesh::VertexData{},Mesh::VertexData{} });
	vertexArray.SetIndexData(std::array{ 0u,1u,2u, 1u,3u,2u });

	// 左下
	vertexArray.GetVertexData()[0u].position = { -0.5f, -0.5f, 0.f, 1.f };
	vertexArray.GetVertexData()[0u].texCoord = { 0.f,1.f };
	vertexArray.GetVertexData()[0u].normal = { 0.f, 0.f, -1.f };
	// 左上
	vertexArray.GetVertexData()[1u].position = { -0.5f, +0.5f, 0.f, 1.f };
	vertexArray.GetVertexData()[1u].texCoord = { 0.f,0.f };
	vertexArray.GetVertexData()[1u].normal = { 0.f, 0.f, -1.f };

	// 右下
	vertexArray.GetVertexData()[2u].position = { +0.5f, -0.5f, 0.f, 1.f };
	vertexArray.GetVertexData()[2u].texCoord = { 1.f,1.f };
	vertexArray.GetVertexData()[2u].normal = { 0.f, 0.f, -1.f };
	// 右上
	vertexArray.GetVertexData()[3u].position = { +0.5f, +0.5f, 0.f, 1.f };
	vertexArray.GetVertexData()[3u].texCoord = { 1.f,0.f };
	vertexArray.GetVertexData()[3u].normal = { 0.f, 0.f, -1.f };

	newModel->materialMap_["default"] = (std::make_unique<Material>());
	auto &material = newModel->materialMap_["default"];

	newModel->name_ = "plane";

	material->Create();

	mesh->SetMaterial(material.get());


	AddModel("Plane", newModel.release());
}
