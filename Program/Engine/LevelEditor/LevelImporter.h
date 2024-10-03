#pragma once
#include "LevelData.h"
#include "../../Engine/ECS/World/World.hpp"
#include "../../Engine/ECS/World/NewWorld.h"

namespace SolEngine {
	class LevelImporter {
	public:


		bool Import(const ResourceObjectManager<LevelData>::Handle levelData, World *const world) const;

		bool Import(const ResourceObjectManager<LevelData>::Handle levelData, ECS::World *const world) const;

		bool Export(ResourceObjectManager<LevelData>::Handle levelData, const World *const world) const;


	private:

		void RecursiveLoad(const std::list<LevelData::ObjectData> &objectDataList, World *const world, ECS::Entity *parent = nullptr) const;
		void RecursiveLoad(const std::list<LevelData::ObjectData> &objectDataList, ECS::World *const world, ECS::EntityClass *parent = nullptr) const;

	};
}