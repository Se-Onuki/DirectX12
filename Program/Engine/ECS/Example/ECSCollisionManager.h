/// @file ECSCollisionManager.h
/// @brief ECS用のコライダマネージャー
/// @author ONUKI seiya
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
		std::list<ECS::ComponentSpan*> collderList_;
		

	};

 }