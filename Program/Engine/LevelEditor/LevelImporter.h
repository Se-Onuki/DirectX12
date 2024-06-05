#pragma once
#include "LevelData.h"
#include "../../ECS/World/World.hpp"

namespace SolEngine {
	class LevelImporter {
	public:


		bool Import(const ResourceObjectManager<LevelData>::Handle levelData, World *const world) const;

		bool Export(ResourceObjectManager<LevelData>::Handle levelData, const World *const world) const;


	private:

		void RecursiveLoad(const std::list<LevelData::ObjectData> &objectDataList, World *const world, ECS::Entity *parent = nullptr) const;

	};
}