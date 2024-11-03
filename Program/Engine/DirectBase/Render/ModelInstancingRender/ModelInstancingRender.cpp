#include "ModelInstancingRender.h"

namespace SolEngine {
	ModelInstancingRender::ModelInstancingRender(uint32_t size)
	{
		// メモリ確保
		Init(size);
	}

	void ModelInstancingRender::Init(uint32_t size)
	{
		buffer_ = std::make_unique<StructuredBuffer<InstanceType>>(size);
	}

	void ModelInstancingRender::Clear()
	{
		buffer_->clear();
	}

	void ModelInstancingRender::SetModelData(ResourceHandle<ModelData> modelData)
	{
		// モデルデータの置き換え
		modelData_ = modelData;
	}

	std::span<ModelInstancingRender::InstanceType> ModelInstancingRender::Reservation(size_t count)
	{
		return buffer_->Reservation(count);
	}

	void ModelInstancingRender::Execute(const Camera3D &camera) const
	{
		modelData_->Draw(buffer_->GetHeapRange().GetHandle().gpuHandle_, buffer_->size(), buffer_->GetStartIndex(), camera);
	}

}