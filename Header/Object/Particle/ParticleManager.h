#pragma once
#include <list>
#include "../Particle.h"
#include "../../../Engine/DirectBase/Base/StructBuffer.h"
#include "../../../Engine/DirectBase/Model/ModelManager.h"

class ParticleList;
class IParticle;

/// <summary>
/// パーティクル基底クラス
/// </summary>
class ParticleManager
{
private: // 

	// シングルトンパターンの設定
	ParticleManager() = default;
	~ParticleManager() = default;
	ParticleManager(const ParticleManager&) = delete;
	const ParticleManager& operator=(const ParticleManager&) = delete;

public:

	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns>シングルトンインスタンス</returns>
	static ParticleManager* GetInstance() {
		static ParticleManager instance;
		return &instance;
	};

	/// <summary>
	/// 初期化関数
	/// </summary>
	/// <param name="maxCount">パーティクル最大数</param>
	void Init(uint32_t maxCount);

	/// <summary>
	/// 更新関数
	/// </summary>
	/// <param name="deltaTime">経過秒数</param>
	void Update(float deltaTime);

	void Draw(const Camera3D& camera);

	void AddParticle(const Model* const modelKey, std::unique_ptr<IParticle> particle);

private: // メンバ変数

	// 粒子配列
	ArrayBuffer<Particle::ParticleData> particles_;

	DescHeap<D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV>::HeapRange heapRange_;

	std::unordered_map<const Model*, std::unique_ptr<ParticleList>> particleMap_;

};


class ParticleList {
public:

	void Update(float deltaTime);

	IParticle* const push_back(std::unique_ptr<IParticle> particle);

	const auto& GetParticleList() { return particles_; }

	uint32_t size() const { return static_cast<uint32_t>(particles_.size()); }

	auto begin() { return particles_.begin(); }
	auto begin() const { return particles_.cbegin(); }

	auto end() { return particles_.end(); }
	auto end() const { return particles_.cend(); }

	void SetModel(const Model* const model) { model_ = model; }

private: // 

	// 
	const Model* model_;
	// 
	std::list<std::unique_ptr<IParticle>> particles_;
};

class IParticle
{
public:
	IParticle() = default;
	virtual ~IParticle() = default;

public:

	virtual void Init() = 0;

	virtual void Update(float deltaTime) = 0;

public:

	const BaseTransform& GetTransform()const { return transform_; }

	const Vector4& GetColor() const { return color_; }

	const bool& GetisAlive() const { return isAlive_; }

protected:

	BaseTransform transform_;
	Vector4 color_;

	bool isAlive_ = true;

};