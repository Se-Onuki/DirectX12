#include "Particle.h"
#include <cstdlib>
#include <new>
#include <malloc.h>

namespace SolEngine::VFX {

	const IParticle::DrawData IParticle::GenerateDrawData() const
	{
		DrawData result;
		OutputDrawData(&result);
		return result;
	}

	std::unique_ptr<std::byte[]> IParticleSystem::GenerateMemory() const
	{
		auto memory = _aligned_malloc(particleDataBase_.size_, particleDataBase_.alignment_);
		std::unique_ptr<std::byte[]> result = std::unique_ptr<std::byte[]>{ static_cast<std::byte *>(memory) };
		return std::move(result);
	}

	void ParticleList::Update(float deltaTime)
	{
		std::for_each(particleData_.begin(), particleData_.end(), [deltaTime](std::unique_ptr<IParticle> &particle)->void
			{
				particle->Update(deltaTime);
			}
		);
		std::erase_if(particleData_, [](const std::unique_ptr<IParticle> &particle) { return particle->IsDead(); });
	}

	void ParticleList::push_back(std::list<std::unique_ptr<IParticle>> &&particle)
	{
		particleData_.insert(particleData_.end(), std::move_iterator(particle.begin()), std::move_iterator(particle.end()));
	}

	void ParticleList::push_back(const std::function<std::unique_ptr<IParticle>(void)> &func, uint32_t count)
	{
		// 生成処理を行う｡
		std::generate_n(std::inserter(particleData_, particleData_.end()), count, func);
	}


	void ParticleEmitter::Init()
	{

	}

	void ParticleEmitter::Start()
	{
		durationTimer_.Start();
		std::for_each(burstEmitter_.begin(), burstEmitter_.end(), [](auto &data) {data.second = false; });
	}

	void ParticleEmitter::Update(float deltaTime)
	{
		durationTimer_.Update(deltaTime);
		if (durationTimer_.IsFinish() and durationTimer_.IsActive()) {
			if (executeCount_) {
				if (*executeCount_ == 0) {
					durationTimer_.Reset();
					isDead_ = true;
				}
				else {
					(*executeCount_)--;
				}
			}

		}
		if (durationTimer_.IsActive()) {
			std::for_each(burstEmitter_.begin(), burstEmitter_.end(), [this](std::pair<Burst, bool> &burstData) {
				if (burstData.second) { return; }	// もし使ったデータなら飛ばす
				if (burstData.first.time_ > durationTimer_.GetNowFlame()) { return; }	// もしまだ到達してないなら飛ばす
				GenerateParticle(burstData.first.count_);	// 指定した個数分追加
				burstData.second = true;	// フラグを立てる
				}
			);

			EmitUpdate(deltaTime);
		}
	}

	void ParticleEmitter::EmitUpdate(float deltaTime)
	{
		if (deltaTime <= 0) { return; }

		float spawnCount = particleSpawnOfTime_ * deltaTime;
		spawnCalcBuffer_ += std::fmodf(spawnCount, 1.f);

		GenerateParticle(static_cast<uint32_t>(spawnCount + spawnCalcBuffer_));
		spawnCalcBuffer_ = std::fmodf(spawnCalcBuffer_, 1.f);
	}

	void ParticleEmitter::SetDurationTimer(float goal, bool isLoop)
	{
		durationTimer_.Start(goal, isLoop);
		durationTimer_.Reset();
	}

	void ParticleEmitter::AddBurst(const Burst burst)
	{
		burstEmitter_.push_back({ burst,false });
		burstEmitter_.sort();
	}

	void ParticleEmitter::GenerateParticle(uint32_t count) const
	{
		if (not count) { return; }	// もし空なら作らない

		particleList_->push_back([this]()
			{
				auto particle = generater_();
				particle->Init();
				dynamic_cast<TestParticle *>(particle.get())->transform_ = emitterTransform_;
				return std::move(particle);
			}, count);
	}

	ParticleEmitter *const ParticleManager::AddEmitter(std::unique_ptr<ParticleEmitter> particleEmitter)
	{
		// 検索タグ
		const auto tag = GeneraterAndModel(particleEmitter->GetGenerater(), particleEmitter->GetModelHandle());
		// データの検索
		auto itr = particleData_.find(tag);
		// タグがあるかどうか
		if (itr == particleData_.end()) {
			// なかったら生成
			auto item = particleData_.emplace(tag, std::make_unique<ParticleList>());
			// イテレータの交換
			itr = item.first;
		}
		// イテレータの中のパーティクルリストをエミッターに渡す
		particleEmitter->SetParticleList(itr->second.get());
		// エミッタのデータを転送して格納する
		return particleEmitter_.emplace_back(std::move(particleEmitter)).get();
	}

	void ParticleManager::Update(float deltaTime)
	{
		std::for_each(particleEmitter_.begin(), particleEmitter_.end(), [deltaTime](auto &emitter) { emitter->Update(deltaTime); });
		std::erase_if(particleEmitter_, [](const auto &emitter) { return emitter->IsDead(); });
		for (auto &[key, data] : particleData_) {
			data->Update(deltaTime);
		}
	}

	std::unique_ptr<ParticleManager> SolEngine::VFX::ParticleManager::Generate()
	{
		auto result = std::make_unique<ParticleManager>();

		return std::move(result);
	}




	void TestParticle::Init()
	{
		color_ = 0xFFFFFFFF;
		velocity_ = Quaternion::Create(SoLib::Math::Euler(-45._deg, SoLib::Random::GetRandom(0.f, SoLib::Angle::Rad360), 0.f)).GetFront() * SoLib::Random::GetRandom(2.f,5.f);

	}

	void TestParticle::Update(float deltaTime)
	{
		acceleration_.y += -9.8f * deltaTime;
		velocity_ += acceleration_;
		transform_.translate_ += velocity_ * deltaTime;
		acceleration_ = {};
		if (transform_.translate_.y < -0.5f and velocity_.y < 0.f) { isDead_ = true; }
	}

	void TestParticle::OutputDrawData(IParticle::DrawData *const drawData) const
	{
		// 座標の転送
		TransferTransform(drawData);
		// 色の転送
		TransferColor(drawData);
	}

	void TestParticle::TransferTransform(DrawData *const drawData) const
	{
		drawData->mat_ = transform_.Affine();
	}

	void TestParticle::TransferColor(DrawData *const drawData) const
	{
		drawData->color_ = color_;
	}

	void ParticleRender::Init()
	{
	}

	void ParticleRender::FrameClear()
	{
		// for文で走査
		for (const auto &[model, datas] : modelBuffers_) {
			datas->clear();
		}

	}

	void ParticleRender::ExecuteDraw(ParticleManager *const particleManager, const Camera3D &camera)
	{
		// データの取得
		const auto &particleData = particleManager->GetParticleData();
		// for文で走査
		for (const auto &[funcAndModel, datas] : particleData) {

			// もし空なら飛ばす
			if (datas->particleData_.empty()) { continue; }
			// モデルデータの取得
			auto &&[func, model] = funcAndModel;
			// モデルデータからインスタンスの取得
			auto buffer = GetOrAddModelBuffer(model);
			// 書き込み先の予約
			auto memory = buffer->Reservation(datas->particleData_.size());

			assert(memory.size() == datas->particleData_.size() and "書き込みデータが範囲外になりました");
			// データの転送
			std::transform(datas->particleData_.begin(), datas->particleData_.end(), memory.begin(), [](const std::unique_ptr<IParticle> &particle) { return particle->GenerateDrawData(); });


		}

		// for文で走査
		for (const auto &[model, datas] : modelBuffers_) {
			// 描画処理
			model->Draw(datas->GetHeapRange().GetHandle().gpuHandle_, datas->size(), datas->GetStartIndex(), camera);
		}

	}

	std::unique_ptr<ParticleRender> ParticleRender::Generate()
	{
		std::unique_ptr<ParticleRender> result = std::make_unique<ParticleRender>();
		result->Init();
		return std::move(result);
	}

	StructuredBuffer<IParticle::DrawData> *ParticleRender::GetOrAddModelBuffer(ModelHandle modelHandle)
	{
		// モデルに応じたイテレータ
		auto itr = modelBuffers_.find(modelHandle);
		// データが空だったら
		if (itr == modelBuffers_.end()) {
			// 構築して渡す
			itr = modelBuffers_.emplace(modelHandle, std::make_unique<StructuredBuffer<IParticle::DrawData>>(bufferSize_)).first;
		}

		// バッファのアドレスを返す
		return itr->second.get();
	}

}