#pragma once
#include <memory>

#include <typeinfo>
#include <concepts>

#include <string>
#include <unordered_map>
#include "../../Utils/Math/Transform.h"
#include "../../Utils/Math/Vector2.h"
#include "../../Utils/Math/Vector3.h"
#include "../../Engine/DirectBase/Model/Model.h"
#include "../../Engine/DirectBase/Model/ModelManager.h"
#include "../../Utils/ConstString.h"

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

	struct IsAlive : IComponent {
		bool isAlive_ = true;
	};

	struct Identifier : IComponent {
		ConstString<8u> name_;
	};

	struct TransformComp : IComponent {
		BaseTransform wTransform_;
	};

	struct PositionComp : IComponent {
		Vector3 position_;
		inline Vector3 &operator=(const Vector3 &other) { position_ = other; }
		inline operator Vector3 &() { return position_; }
		inline operator const Vector3 &() const { return position_; }
	};

	struct ModelComp : IComponent {
		void Init(const std::string &key) {
			model_ = ModelManager::GetInstance()->GetModel(key);
		}
		Model *model_;

	};

	struct InputFlagComp : IComponent {};

	template<class T>
	constexpr bool cIsComponentData = std::is_base_of_v<IComponent, T> &&std::is_trivial_v<T> &&std::is_trivially_destructible_v<T>;
}