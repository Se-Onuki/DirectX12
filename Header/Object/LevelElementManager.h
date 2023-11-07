#pragma once
#include <list>
#include <memory>

#include "../../Engine/DirectBase/Render/Camera.h"
#include "../Collision/Collision.h"
#include "../../Engine/DirectBase/Model/Model.h"

class LevelElementManager {
	LevelElementManager() = default;
	LevelElementManager(const LevelElementManager &) = delete;
	LevelElementManager &operator=(const LevelElementManager &) = delete;
	~LevelElementManager() = default;
public:

	class Box {
	public:
		Box(const AABB &aabb);

		Transform transform_;
		AABB referenceBox_;
	};

	class Platform {
	public:
		Platform() = default;
		~Platform() = default;

		BaseTransform center_;

		void AddBox(const AABB &box);
		void CalcCollision();

		void Draw(const Model *const model, const Camera3D &camera) const;

		const auto &GetCollider() const { return collisionBox_; }

	private:
		std::list<AABB> collisionBox_;

		std::list<Box> boxList_;
	};

public:

	static auto *const GetInstance() {
		static LevelElementManager instance{};
		return &instance;
	}

	void Init();

	void Update(float deltaTime);

	void Draw(const Camera3D &camera) const;

	void Draw(const Camera3D &camera);

	void CalcCollision(const uint32_t key);

	/// @brief ブロックを追加
	/// @param transform ブロックのSRT
	void AddBlock(const uint32_t key, const AABB &box);


	std::unordered_map<uint32_t, Platform> blockCollider_;
private:

};
//
//template <SoLib::IsContainer T>
//inline void LevelElementManager::AddBlock(const std::string &key, T &entities) {
//	static_assert(std::is_same_v<typename T::value_type, Entity *>, "与えられた引数はEntity*型を持つコンテナ型ではありません");
//	for (Entity *entity : entities) {
//		AddBlock(key, entity);
//	}
//}