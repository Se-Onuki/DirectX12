#pragma once
#include "SceneManager.h"

#include "../Engine/DirectBase/3D/DirectionLight.h"
#include "../Engine/DirectBase/2D/Sprite.h"
#include "../Engine/DirectBase/Render/CameraAnimations/CameraManager.h"

#include "../Engine/DirectBase/Model/Model.h"
#include "../Utils/Math/Transform.h"
#include "../Header/Object/Particle.h"

#include "../Engine/DirectBase/Input/Input.h"
#include "../Engine/DirectBase/Base/Audio.h"
#include "../ECS/World/World.hpp"
#include "../ECS/Entity/EntityManager.hpp"
#include "../ECS/MultiArray.h"

#include "../Entt/entt.hpp"

class GameScene : public IScene {
public:
	GameScene();
	~GameScene();

	void OnEnter() override;
	void OnExit() override;

	void Update() override;
	void Draw() override;

private:

	Input *input_ = nullptr;
	Audio *audio_ = nullptr;
	CameraManager *cameraManager_ = nullptr;

	std::unique_ptr<DirectionLight> light_ = nullptr;

	World *world_ = nullptr;

	std::unique_ptr<ECS::MultiArray> mArray_;

	std::unique_ptr<entt::registry> registry_;

	// 粒子配列
	std::unique_ptr<ArrayBuffer<Particle::ParticleData>> particles_;
	uint32_t particleIndex_ = 0u;
	// ヒープレンジ
	DescHeap<D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV>::HeapRange heapRange_;

	CBuffer<uint32_t> indexBegin_;

	Model *model_;

	Camera3D camera_;
};



inline void EnttUpdate(entt::registry &registry) {
	auto view = registry.view<ECS::Component::Position, ECS::Component::Velocity, ECS::Component::TransformMatrix>();

	// use a range-for
	for (auto [entity, pos, vel, matrix] : view.each()) {
		pos.position_ += vel;
		matrix = Matrix4x4::Affine(Vector3::one, Vector3::zero, pos);
	}
}

inline void EnttDraw(ArrayBuffer<Particle::ParticleData> *particleArray, uint32_t &index, entt::registry &registry) {
	auto view = registry.view<ECS::Component::Color, ECS::Component::TransformMatrix>();

	index = 0u;
	// use forward iterators and get only the components of interest
	for (auto [entity, color, matrix] : view.each()) {
		auto &item = (*particleArray)[index++];
		item.transform.World = matrix;
		item.color = color;
	}
}
