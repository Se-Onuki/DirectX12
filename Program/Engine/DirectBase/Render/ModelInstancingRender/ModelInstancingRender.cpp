/// @file ModelInstancingRender.cpp
/// @brief 単一モデルのインスタンス描画
/// @author ONUKI seiya
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

	void ModelInstancingRender::AddMatData(const ECS::World &world, const Archetype &arch, const uint32_t color, void(*afterFunc)(InstanceType &))
	{

		// チャンクの取得
		auto ghostChanks = world.GetAccessableChunk(arch);
		// オブジェクトの総数
		uint32_t totalCount = 0u;

		// チャンクと同じ数のデータを確保する
		std::vector<uint32_t> ghostOffset(ghostChanks.size());
		for (uint32_t i = 0; i < ghostOffset.size(); i++) {
			ghostOffset[i] = totalCount;
			totalCount += ghostChanks[i]->size();
		}
		// もし空なら終わる
		if (totalCount == 0) {
			return;
		}

		// 書き込み先の確保
		auto span = Reservation(totalCount);
		for (uint32_t i = 0; i < ghostOffset.size(); i++) {
			// チャンクからデータの取得
			auto transMats = ghostChanks[i]->GetComponent<ECS::TransformMatComp>();
			// 転送する
			std::transform(transMats.begin(), transMats.end(), &span[ghostOffset[i]], [color, afterFunc](const ECS::TransformMatComp &trans) { Particle::ParticleData result{ .transform = trans.transformMat_, .color = color };
			if (afterFunc) {
				afterFunc(result);
			}
			return result;
				});
		}
	}
	void ModelInstancingRender::AddTransData(const ECS::World &world, const Archetype &arch, const uint32_t color, std::function<void(InstanceType &)> afterFunc)
	{

		// チャンクの取得
		auto ghostChanks = world.GetAccessableChunk(arch);
		// オブジェクトの総数
		uint32_t totalCount = 0u;

		// チャンクと同じ数のデータを確保する
		std::vector<uint32_t> ghostOffset(ghostChanks.size());
		for (uint32_t i = 0; i < ghostOffset.size(); i++) {
			ghostOffset[i] = totalCount;
			totalCount += ghostChanks[i]->size();
		}
		// もし空なら終わる
		if (totalCount == 0) {
			return;
		}

		// 書き込み先の確保
		auto span = Reservation(totalCount);
		for (uint32_t i = 0; i < ghostOffset.size(); i++) {
			// チャンクからデータの取得
			auto transMats = ghostChanks[i]->GetComponent<ECS::PositionComp>();
			// 転送する
			std::transform(transMats.begin(), transMats.end(), &span[ghostOffset[i]], [color, afterFunc](const ECS::PositionComp &trans) {

				Particle::ParticleData result{ .color = color };
				result.transform.World = Matrix4x4::Identity();
				result.transform.World.GetTranslate() = trans;

				if (afterFunc) {
					afterFunc(result);
				}
				return result;
				});
		}
	}

	void ModelInstancingRender::DrawExecute(const Camera3D &camera) const
	{
		modelData_->Draw(buffer_->GetHeapRange().GetHandle().gpuHandle_, buffer_->size(), buffer_->GetStartIndex(), camera);
	}

}