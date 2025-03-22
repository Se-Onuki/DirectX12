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
	}

	void ParticleList::push_back(std::list<std::unique_ptr<IParticle>> &&particle)
	{
		particleData_.insert(particleData_.end(), std::move_iterator(particle.begin()), std::move_iterator(particle.end()));
	}

	void ParticleList::push_back(std::unique_ptr<IParticle>(*particle)(), uint32_t count)
	{
		//particle; count;
		std::generate_n(std::inserter(particleData_, particleData_.end()), count, particle);
	}


	void ParticleEmitter::Init()
	{

	}

	void ParticleEmitter::Start()
	{
		timer_.Start();
	}

	void ParticleEmitter::Update(float deltaTime)
	{
		timer_.Update(deltaTime);
		if (timer_.IsFinish() and timer_.IsActive()) {
			if (executeCount_) {
				if (*executeCount_ == 0) {
					timer_.Reset();
				}
			}
			EmitUpdate(deltaTime);

		}
	}

	void ParticleEmitter::EmitUpdate(float deltaTime) const
	{
		if (deltaTime <= 0) { return; }

		GenerateParticle(static_cast<uint32_t>(particleSpawnOfTime_ / deltaTime));
	}

	void ParticleEmitter::GenerateParticle(uint32_t count) const
	{
		particleList_->push_back(generater_, count);
	}

	void ParticleManager::AddEmitter(std::unique_ptr<ParticleEmitter> particleEmitter)
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
		particleBuffer_.push_back(std::move(particleEmitter));
	}

	void ParticleManager::Update(float deltaTime)
	{
		std::for_each(particleBuffer_.begin(), particleBuffer_.end(), [deltaTime](auto &buffer) { buffer->Update(deltaTime); });
	}

	std::unique_ptr<ParticleManager> SolEngine::VFX::ParticleManager::Generate()
	{
		auto result = std::make_unique<ParticleManager>();

		return std::move(result);
	}




	void TestParticle::Init()
	{

	}

	void TestParticle::Update(float deltaTime)
	{
		transform_.translate_ += velocity_ * deltaTime;
	}

	void TestParticle::OutputDrawData(IParticle::DrawData *const drawData) const
	{
		// 座標の計算処理
		TransferTransform(drawData);
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
			itr = modelBuffers_.emplace(modelHandle, std::make_unique<StructuredBuffer<IParticle::DrawData>>(1024)).first;
		}

		// バッファのアドレスを返す
		return itr->second.get();
	}

}