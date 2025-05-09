/// @file Particle.h
/// @brief Particleの実装
/// @author ONUKI seiya
#pragma once
#include <cstdint>
#include "../../Utils/Math/Math.hpp"
#include "../../Engine/ResourceObject/ResourceObjectManager.h"

#include "../Engine/DirectBase/Base/EngineObject.h"
#include "../Engine/DirectBase/Base/StructBuffer.h"

#include "../Engine/ECS/World/NewWorld.h"

namespace SolEngine::VFX
{

	class IParticle {
	public:
		IParticle() = default;
		virtual ~IParticle() {};

		/// @brief 描画データ
		struct DrawData {
			/// @brief ワールド座標データ
			Matrix4x4 mat_;
			/// @brief 色
			SoLib::Color::RGB4 color_;
		};

		/// @brief 初期化処理
		virtual void Init() = 0;

		/// @brief 更新処理
		/// @param[in] deltaTime 秒差分
		virtual void Update(float deltaTime) = 0;

		/// @brief データの書き出し
		/// @param[out] drawData 書き出し先
		virtual void OutputDrawData(DrawData *const drawData) const = 0;

		/// @brief 死亡判定
		/// @return 死亡していたならtrue
		virtual bool IsDead() const = 0;

		/// @brief 書き出し処理のGenerate版
		/// @return 生成したデータ
		const DrawData GenerateDrawData() const;
	};

	class IParticleSystem {
		// モデルのハンドル
		using ModelHandle = SolEngine::ResourceHandle<SolEngine::ModelData>;
	public:
		virtual ~IParticleSystem() = default;

		/// @brief 描画データ
		struct DrawData final {
			/// @brief ワールド座標データ
			Matrix4x4 mat_;
			/// @brief 色
			SoLib::Color::RGB4 color_;
		};

		struct ParticleDataBase final {
			uint32_t alignment_;
			uint32_t size_;

			template<typename T>
			static ParticleDataBase Generate() {
				return ParticleDataBase{
					.alignment_ = alignof(T),
					.size_ = sizeof(T),
				};
			}
		};


		/// @brief 初期化処理
		virtual void Init(std::byte *const data) const = 0;

		/// @brief 更新処理
		/// @param[in] deltaTime 秒差分
		virtual void Update(float deltaTime, std::byte *const data) const = 0;

		/// @brief データの書き出し
		/// @param[out] drawData 書き出し先
		virtual void OutputDrawData(DrawData *const drawData, const std::byte *const data) const = 0;

		/// @brief メモリの生成
		/// @return 生成したデータ
		std::unique_ptr<std::byte[]> GenerateMemory() const;


		ParticleDataBase particleDataBase_;
		// ParticleDataBase sharedData_;

	};

	template<typename T>
	class ParticleSystemTemplate final : public IParticleSystem {
	private:

		T &Cast(std::byte *const data) const {
			return *static_cast<T *const>(data);
		}
		const T &Cast(const std::byte *const data) const {
			return *static_cast<T *const>(data);
		}

		ParticleSystemTemplate() {
			particleDataBase_ = ParticleDataBase::Generate<T>();
		}
	public:

		/// @brief 初期化処理
		void Init(std::byte *const data) const override {
			Cast(data).Init();
		}

		/// @brief 更新処理
		/// @param[in] deltaTime 秒差分
		void Update(float deltaTime, std::byte *const data) const override {
			Cast(data).Update(deltaTime);
		}

		/// @brief データの書き出し
		/// @param[out] drawData 書き出し先
		void OutputDrawData(DrawData *const drawData, const std::byte *const data) const override {
			Cast(data).OutputDrawData(drawData);
		}

	};

	class TestParticle : public IParticle {
	public:
		TestParticle() = default;
		~TestParticle() override {};

		void Init() override;

		void Update(float deltaTime) override;

		void OutputDrawData(DrawData *const drawData) const override;

		bool IsDead() const override { return isDead_; }

	private:

		/// @brief 座標の計算処理
		/// @param[out] drawData 書き込み先
		void TransferTransform(DrawData *const drawData) const;

		/// @brief 色の書き込み処理
		/// @param[out] drawData 書き込み先
		void TransferColor(DrawData *const drawData) const;

	public:
		// 加速度
		Vector3 acceleration_;
		// 速度
		Vector3 velocity_;
		// 座標系
		SoLib::SimpleTransformQuaternion transform_;
		// 色
		SoLib::Color::RGB4 color_;

		bool isDead_ = false;

	};

	class ParticleList {
	public:

		void Update(float deltaTime);

		void push_back(std::list<std::unique_ptr<IParticle>> &&particle);

		void push_back(const std::function<std::unique_ptr<IParticle>(void)> &func, uint32_t count);

	public:

		std::list<std::unique_ptr<IParticle>> particleData_;

	};

	class ParticleEmitter {

		// モデルのハンドル
		using ModelHandle = SolEngine::ResourceHandle<SolEngine::ModelData>;
	public:
		/// @brief 発射するデータ
		struct Burst {
			float time_;
			uint32_t count_;
			bool operator==(const Burst &) const = default;
			auto operator<=>(const Burst &that) const {
				if (std::partial_ordering comp = this->time_ <=> that.time_; comp != 0) { return comp; }
				std::partial_ordering comp = this->count_ <=> that.count_;
				return comp;
			}
		};

	public:

		void Init();

		void Start();

		void Update(float deltaTime);

		void EmitUpdate(float deltaTime);

		void SetParticleList(ParticleList *const particleList) { particleList_ = particleList; }

		void SetModelHandle(const ModelHandle model) { modelData_ = model; }

		void SetSpawnCount(uint32_t count) { particleSpawnOfTime_ = count; }

		void SetDurationTimer(float goal, bool isLoop = false);

		void AddBurst(const Burst burst);

		void SetScale(const Vector3 &scale) { emitterTransform_.scale_ = scale; }
		void SetTranslate(const Vector3 &translate) { emitterTransform_.translate_ = translate; }

		const auto &GetTransform() const { return emitterTransform_; }

		ModelHandle GetModelHandle() const { return modelData_; }

		/// @brief ジェネレーターの取得
		/// @return ジェネレーターの関数ポインタ
		auto GetGenerater() const { return generater_; }

		bool IsDead() const { return isDead_; }

		template<typename T>
		void SetGenerater() { generater_ = []()->std::unique_ptr<IParticle> { return std::make_unique<T>(); }; }

		template<typename T>
		static std::unique_ptr<ParticleEmitter> Generate();

	private:

		/// @brief パーティクルの生成処理
		/// @param[in] count 生成数
		void GenerateParticle(uint32_t count) const;

		// 生成関数
		std::unique_ptr<IParticle>(*generater_)() = nullptr;

		// パーティクルの格納先
		ParticleList *particleList_ = nullptr;

		std::list<std::pair<Burst, bool>> burstEmitter_;

		// 実行のタイマー
		SoLib::Time::DeltaTimer durationTimer_;

		// パーティクル生成の個数
		uint32_t particleSpawnOfTime_ = 0;

		// モデルデータ
		ModelHandle modelData_;

		/// @brief 残り実行回数(nulloptは無限)
		std::optional<uint32_t> executeCount_ = std::nullopt;

		// エミッタの座標
		SoLib::SimpleTransformQuaternion emitterTransform_;

		// 生成数の時間の蓄積
		float spawnCalcBuffer_ = 0.f;

		/// @brief 死んでいるか否か
		bool isDead_ = false;
	};

	class ParticleManager : EngineObject {

		// モデルのハンドル
		using ModelHandle = SolEngine::ResourceHandle<SolEngine::ModelData>;
	public:
		struct GeneraterAndModel {
			std::unique_ptr<IParticle>(*generater_)();
			ModelHandle model_;
			bool operator==(const GeneraterAndModel &) const = default;
		};
		struct Hash {
			size_t operator()(const GeneraterAndModel &data) const {
				return (data.model_.GetHashID() & 0xFFFFFFFF) | (std::bit_cast<size_t>(data.generater_) & 0xFFFFFFFF00000000);
			}
		};

	public:

		ParticleManager() = default;

		ParticleEmitter *const AddEmitter(std::unique_ptr<ParticleEmitter> particleEmitter);

		void Update(float deltaTime);

		static std::unique_ptr<ParticleManager> Generate();

		const auto &GetParticleData() const { return particleData_; }

	private:

		std::unordered_map<GeneraterAndModel, std::unique_ptr<ParticleList>, Hash> particleData_;

		std::list<std::unique_ptr<ParticleEmitter>> particleEmitter_;
	};

	class ParticleRender : EngineObject {
	private:

		// モデルのハンドル
		using ModelHandle = SolEngine::ResourceHandle<SolEngine::ModelData>;

		struct Hash {
			size_t operator()(const ModelHandle model) const {
				return model.GetHashID();
			}
		};

	public:

		/// @brief 初期化処理
		void Init();

		/// @brief フレーム単位の初期化
		void FrameClear();

		/// @brief 描画処理
		/// @param[in] particleManager データの取得元
		void ExecuteDraw(ParticleManager *const particleManager, const Camera3D &camera);

		static std::unique_ptr<ParticleRender> Generate();

	private:

		/// @brief モデルデータに応じたバッファ取得
		/// @param[in] modelHandle モデルのハンドル
		StructuredBuffer<IParticle::DrawData> *GetOrAddModelBuffer(ModelHandle modelHandle);

	private:

		std::unordered_map<ModelHandle, std::unique_ptr<StructuredBuffer<IParticle::DrawData>>, Hash> modelBuffers_;

		uint32_t bufferSize_ = 1024;

	};



	template<typename T>
	inline std::unique_ptr<ParticleEmitter> ParticleEmitter::Generate()
	{
		std::unique_ptr<ParticleEmitter> result = std::make_unique<ParticleEmitter>();

		result->SetGenerater<T>();
		result->Init();

		return std::move(result);
	}

}