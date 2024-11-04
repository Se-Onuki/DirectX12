#pragma once
#include "../../Model/ModelData.h"
#include "../../../../Header/Object/Particle.h"
#include "../../Engine/ECS/World/NewWorld.h"

namespace SolEngine {

	class ModelInstancingRender
	{
	public:
		ModelInstancingRender() = default;
		ModelInstancingRender(uint32_t size);
		~ModelInstancingRender() = default;

		using InstanceType = Particle::ParticleData;

	public:
		/// @brief バッファの初期化
		/// @param size 初期化
		void Init(uint32_t size);

		/// @brief 保存しているデータの破棄
		void Clear();

		/// @brief モデルデータの指定
		/// @param modelData モデルデータ
		void SetModelData(ResourceHandle<ModelData> modelData);

		/// @brief 描画データの書き込み先の取得
		/// @param count 取得数
		/// @return 取得した書き込み先の配列
		std::span<InstanceType> Reservation(size_t count);

		template<typename T>
		void AddMatData(const ECS::World &world, const uint32_t color = 0xFFFFFFFF, void(*afterFunc)(InstanceType &) = nullptr);

		void AddMatData(const ECS::World &world, const Archetype &arch, const uint32_t color = 0xFFFFFFFF, void(*afterFunc)(InstanceType &) = nullptr);

		/// @brief 描画の実行
		/// @param camera カメラ情報
		void DrawExecute(const Camera3D &camera) const;

	private:


		// モデルデータ
		ResourceHandle<ModelData> modelData_;

		// 描画データ
		std::unique_ptr<StructuredBuffer<InstanceType>> buffer_;


	};

	template<typename T>
	inline void ModelInstancingRender::AddMatData(const ECS::World &world, const uint32_t color, void(*afterFunc)(InstanceType &))
	{
		Archetype archetype;
		archetype.AddClassData<T>();

		AddMatData(world, archetype, color, afterFunc);

	}

}