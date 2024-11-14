#pragma once
#include "../World/ComponentArray/ComponentData.h"
#include <list>

namespace ECS {

	class CollisionManager
	{
	public:
		CollisionManager();
		~CollisionManager();

	private:

		// コライダのリスト
		std::list<ECS::ComponentData*> collderList_;
		

	};

 }