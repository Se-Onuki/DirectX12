/// @file ModelInstancingRender.h
/// @brief 単一モデルのインスタンシング描画
/// @author ONUKI seiya
#pragma once
#include "../../Model/ModelData.h"
#include "../../../../Header/Object/Particle.h"
#include "../../Engine/ECS/World/NewWorld.h"
#include <functional>

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
		void Init(uint32_t size = 2048u);

		/// @brief 保存しているデータの破棄
		void Clear();

		/// @brief モデルデータの指定
		/// @param modelData モデルデータ
		void SetModelData(ResourceHandle<ModelData> modelData);

		/// @brief モデルデータの取得
		/// @return モデルデータ
		ResourceHandle<ModelData> GetModelData() const { return modelData_; }

		/// @brief 描画データの書き込み先の取得
		/// @param count 取得数
		/// @return 取得した書き込み先の配列
		std::span<InstanceType> Reservation(size_t count);

		/// @brief 描画データの追加
		/// @param[in] world ECSのワールド
		/// @param[in] color モデルの色
		/// @param[in] afterFunc 追加後の処理
		template<typename T>
		void AddMatData(const ECS::World &world, const uint32_t color = 0xFFFFFFFF, void(*afterFunc)(InstanceType &) = nullptr);

		/// @brief 描画データの追加
		/// @param[in] world ECSのワールド
		/// @param[in] color モデルの色
		/// @param[in] afterFunc 追加後の処理
		template<typename T>
		void AddTransData(const ECS::World &world, const uint32_t color = 0xFFFFFFFF, std::function<void(InstanceType &)> afterFunc = nullptr);

		/// @brief 描画データの追加
		/// @param[in] world ECSのワールド
		/// @param[in] color モデルの色
		/// @param[in] afterFunc 追加後の処理
		template<typename ...Ts>
			requires(sizeof...(Ts) > 0)
		void AddData(const ECS::World &world, const uint32_t color = 0xFFFFFFFF, std::function<void(InstanceType &)> afterFunc = nullptr);

		/// @brief 描画データの追加
		/// @param[in] world ECSのワールド
		/// @param[in] color モデルの色
		/// @param[in] afterFunc 追加後の処理
		void AddMatData(const ECS::World &world, const Archetype &arch, const uint32_t color = 0xFFFFFFFF, void(*afterFunc)(InstanceType &) = nullptr);

		/// @brief 描画データの追加
		/// @param[in] world ECSのワールド
		/// @param[in] color モデルの色
		/// @param[in] afterFunc 追加後の処理
		void AddTransData(const ECS::World &world, const Archetype &arch, const uint32_t color = 0xFFFFFFFF, std::function<void(InstanceType &)> afterFunc = nullptr);

		/// @brief 描画データの転送 (環境によっては関数に静的検査のエラー表示が出るが､実行･コンパイルは通る)
		/// @tparam ...Ts 抜き出すコンポーネント
		/// @param world ワールドデータ
		/// @param func 書き込み用の変換関数 (ラムダ関数でもコンパイルは通るが､静的検査で誤検知のエラーが出る｡)
		template<typename... Ts>
		void TransfarData(const ECS::World &world, const ECS::ComponentRegistry::ComponentFlag exclusions, std::function<InstanceType(const std::tuple<const Ts &...> &)> func) {

			// チャンクの取得
			auto chanks = world.GetAccessableChunk(Archetype::Generate<Ts...>(), exclusions);
			// オブジェクトの総数
			uint32_t totalCount = 0u;

			// チャンクと同じ数のデータを確保する
			std::vector<uint32_t> offset(chanks.size());
			for (uint32_t i = 0; i < offset.size(); i++) {
				offset[i] = totalCount;
				totalCount += chanks[i]->size();
			}
			// もし空なら終わる
			if (totalCount == 0) {
				return;
			}

			// 書き込み先の確保
			auto span = Reservation(totalCount);
			for (uint32_t i = 0; i < offset.size(); i++) {
				// チャンクからデータの取得
				auto transMats = chanks[i]->View<Ts...>();

				// 転送する
				std::transform(transMats.begin(), transMats.end(), &span[offset[i]], func);

			}
		}

		/// @brief 描画データの転送 (環境によっては関数に静的検査のエラー表示が出るが､実行･コンパイルは通る)
		/// @tparam ...Ts 抜き出すコンポーネント
		/// @param world ワールドデータ
		/// @param 
		/// @param func 書き込み用の変換関数 (ラムダ関数でもコンパイルは通るが､静的検査で誤検知のエラーが出る｡)
		template<typename... Ts>
		void TransfarData(const ECS::World &world, std::function<InstanceType(const std::tuple<const Ts &...> &)> func) {
			TransfarData<Ts...>(world, {}, func);
		}

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
	template<typename T>
	inline void ModelInstancingRender::AddTransData(const ECS::World &world, const uint32_t color, std::function<void(InstanceType &)> afterFunc)
	{
		Archetype archetype;
		archetype.AddClassData<T>();

		AddTransData(world, archetype, color, afterFunc);

	}

}