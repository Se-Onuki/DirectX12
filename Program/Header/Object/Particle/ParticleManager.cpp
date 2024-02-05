#include "ParticleManager.h"
#include "../../../Engine/DirectBase/Base/DirectXCommon.h"
#include "../../Engine/DirectBase/Render/CameraAnimations/CameraManager.h"

void ParticleManager::Init(uint32_t maxCount)
{
	if (particles_ == nullptr) {
		particles_ = std::make_unique<ArrayBuffer<Particle::ParticleData>>(maxCount);
		//particles_.CreateBuffer(maxCount);

		// デバイスの取得
		auto *device = DirectXCommon::GetInstance()->GetDevice();
		// SRVヒープの取得
		auto *srvHeap = DirectXCommon::GetInstance()->GetSRVHeap();

		heapRange_ = srvHeap->RequestHeapAllocation(1);
		device->CreateShaderResourceView(particles_->GetResources(), &particles_->GetDesc(), heapRange_.GetHandle(0).cpuHandle_);
	}

	this->particleMap_.clear();
}

void ParticleManager::Update(float deltaTime)
{
	for (auto &[model, particleList] : particleMap_) {
		particleList->Update(deltaTime);
	}
}

void ParticleManager::Draw(const Camera3D &camera) {
	// 始点となる添え字
	uint32_t index = 0u;
	// サイズと始点を設定
	std::list<std::pair<uint32_t, uint32_t>> sizeAndIndex;
	// パーティクルのモデルと集合を1つづつ取得
	for (auto &[model, particleList] : particleMap_) {
		// パーティクルのリストを取得
		const auto &particles = particleList->GetParticleList();
		// もし、サイズが0なら飛ばす
		if (particles.size() == 0u) { continue; }
		// パーティクルの書き込み先を設定 
		uint32_t targetIndex = index;
		// パーティクルをひとつづつ取得
		for (const auto &particle : particles) {
			// 書き込み先にパーティクルのデータを渡す
			(*particles_)[targetIndex].transform.World = particle->GetTransform().matWorld_;
			(*particles_)[targetIndex].color = particle->GetColor();
			targetIndex++;
		}
		// パーティクルの量と先頭位置を設定
		sizeAndIndex.push_back({ static_cast<uint32_t>(particles.size()), index });
		particleList->SetLocation(index);
		// 次の先頭位置を更新
		index += static_cast<uint32_t>(particles.size());
	}
	// 量と先頭位置のリストのイテレータ
	std::list<std::pair<uint32_t, uint32_t>>::const_iterator indexItr = sizeAndIndex.cbegin();
	// GPUアドレスを取得
	const auto &gpuAddress = heapRange_.GetHandle(0u).gpuHandle_;

	// 構造体のサイズ
	//const uint32_t matrixSize = sizeof(decltype(particles_)::map_matrix);

	// パーティクルのモデルと集合を1つづつ取得
	for (const auto &[model, particleList] : particleMap_) {

		// もし、サイズが0なら飛ばす
		if (particleList->GetParticleList().size() == 0u) { continue; }
		// インスタンシングで描画。
		model->Draw(gpuAddress, indexItr->first, particleList->GetLoaction(), camera);
		// 次の先頭位置に更新
		indexItr++;
	}

}

IParticle *ParticleManager::AddParticle(const Model *const modelKey, std::unique_ptr<IParticle> particle) {
	// パーティクルリストがなかった場合作成
	decltype(particleMap_)::iterator particleListItr = particleMap_.find(modelKey);
	if (particleListItr == particleMap_.end()) {
		particleMap_[modelKey] = std::make_unique<ParticleList>();
		particleMap_[modelKey]->SetModel(modelKey);
	}
	// パーティクルを追加
	return particleMap_[modelKey]->push_back(std::move(particle));
}

void ParticleList::Update(float deltaTime)
{
	particles_.remove_if([](std::unique_ptr<IParticle> &particle)
		{
			if (not particle->GetisAlive()) {
				return true;
			}
			return false;
		});
	const Matrix4x4 &billboardMat = CameraManager::GetInstance()->GetUseCamera()->matView_.GetRotate().Transpose();

	for (auto &particle : particles_) {
		particle->Update(deltaTime);
		particle->transform_.matWorld_ *= billboardMat;
		*reinterpret_cast<Vector3 *>(particle->transform_.matWorld_.m[3]) = particle->GetTransform().translate;
	}
}

IParticle *const ParticleList::push_back(std::unique_ptr<IParticle> particle) {
	particles_.push_back(std::move(particle));
	IParticle *const particlePtr = particles_.back().get();
	particlePtr->Init();
	return particlePtr;
}
