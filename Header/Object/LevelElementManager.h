#pragma once
#include <list>
#include <memory>

#include "../../Engine/DirectBase/Render/Camera.h"
#include "../Entity/Entity.h"

class LevelElementManager
{
public:
	LevelElementManager() = default;
	~LevelElementManager() = default;

	void Init();

	void Update(float deltaTime);

	void Draw(const Camera3D &camera) const;

	/// @brief ブロックを追加
	/// @param transform ブロックのSRT
	void AddBlock(const BaseTransform<false> &transform);

	/// @brief ブロックを追加
	/// @param entity ブロックエンティティ
	void AddBlock(Entity *const entity);

	template <SoLib::IsContainer T>
	void AddBlock(T &entities);

private:

	std::list<std::unique_ptr<Entity>> block_;

};

template <SoLib::IsContainer T>
inline void LevelElementManager::AddBlock(T &entities) {
	static_assert(std::is_same_v<typename T::value_type, Entity *>, "与えられた引数はEntity*型を持つコンテナ型ではありません");
	for (Entity *entity : entities) {
		AddBlock(entity);
	}
}