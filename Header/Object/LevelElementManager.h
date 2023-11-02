#pragma once
#include <list>
#include <memory>

#include "../../Engine/DirectBase/Render/Camera.h"
#include "../Collision/Collision.h"
#include "../../Engine/DirectBase/Model/Model.h"

class LevelElementManager
{
public:

	class Platform {
	public:
		Platform() = default;
		~Platform() = default;

		Vector3 rotate_;
		Vector3 origin_;

		void AddBox(const AABB &box);
		void CalcCollision();

		void Draw(const Model *const model, const Camera3D &camera) const;

	private:
		std::list<Transform> transform_;
		std::list<AABB> referenceBox_;
		std::list<AABB> collisionBox_;
	};

public:
	LevelElementManager() = default;
	~LevelElementManager() = default;

	void Init();

	void Update(float deltaTime);

	void Draw(const Camera3D &camera) const;

	void Draw(const Camera3D &camera);

	void CalcCollision(const uint32_t key);

	/// @brief ブロックを追加
	/// @param transform ブロックのSRT
	void AddBlock(const uint32_t key, const AABB &box);

private:

	std::unordered_map<uint32_t, Platform> blockCollider_;

};
//
//template <SoLib::IsContainer T>
//inline void LevelElementManager::AddBlock(const std::string &key, T &entities) {
//	static_assert(std::is_same_v<typename T::value_type, Entity *>, "与えられた引数はEntity*型を持つコンテナ型ではありません");
//	for (Entity *entity : entities) {
//		AddBlock(key, entity);
//	}
//}