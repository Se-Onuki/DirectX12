#pragma once
#include <memory>

#include <string>
#include <unordered_map>
#include "../../Utils/Math/Transform.h"
#include "../../Utils/Math/Vector2.h"
#include "../../Utils/Math/Vector3.h"
#include "../../Engine/DirectBase/Model/Model.h"
#include "../../Engine/DirectBase/Model/ModelManager.h"

// class SpriteManager {
// public:
//	static SpriteManager& getInstance() {
//		static SpriteManager instance;
//		return instance;
//	}
//
//	~SpriteManager() {
//		for (auto& pair : sprites) {
//			delete pair.second;
//		}
//	}
//
//	Sprite* getSprite(const std::string& key) {
//		auto it = sprites.find(key);
//		if (it != sprites.end()) {
//			return it->second;
//		}
//		return nullptr;
//	}
//
//	void addSprite(const std::string& key, Sprite* sprite) { sprites[key] = sprite; }
//
// private:
//	SpriteManager() {}
//	std::unordered_map<std::string, Sprite*> sprites;
// };

namespace ECS {

	struct IComponent {};

	struct Identifier : IComponent {
		char name_;
		void Init(const char &value) {
			name_ = value;
		}
	};

	struct TransformComp : IComponent {
		BaseTransform wTransform_;
	};

	struct ModelComp : IComponent {
		void Init(const std::string &key) {
			model_ = ModelManager::GetInstance()->GetModel(key);
		}
		Model *model_;
	};

	struct InputFlagComp : IComponent {};
}