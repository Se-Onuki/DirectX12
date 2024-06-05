#pragma once
#include "LevelData.h"
#include "../../ECS/World/World.hpp"

namespace SolEngine {
	class LevelImporter {
	public:


		bool Import(const LevelData *const levelData, World *const world) const;


	private:


	};
}