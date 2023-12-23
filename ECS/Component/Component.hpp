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
#include "../../Utils/Math/Euler.h"

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
		SoLib::ConstString<8u> name_;
	};

	struct AliveTime : IComponent {
		SoLib::Time::SecondF aliveTime_{};
	};

	/*struct TransformComp : IComponent {
		BaseTransform wTransform_;
	};*/

	struct PositionComp : IComponent {
		Vector3 position_;
		inline Vector3 &operator=(const Vector3 &other) { position_ = other; }
		inline operator Vector3 &() { return position_; }
		inline operator const Vector3 &() const { return position_; }
	};

	struct VelocityComp : IComponent {
		Vector3 velocity_;
		inline Vector3 &operator=(const Vector3 &other) { velocity_ = other; }
		inline operator Vector3 &() { return velocity_; }
		inline operator const Vector3 &() const { return velocity_; }
	};

	struct ScaleComp : IComponent {
		Vector3 scale_;
		inline Vector3 &operator=(const Vector3 &other) { scale_ = other; }
		inline operator Vector3 &() { return scale_; }
		inline operator const Vector3 &() const { return scale_; }
	};

	struct RotateComp : IComponent {
		SoLib::Math::Euler rotate_;
		inline SoLib::Math::Euler &operator=(const SoLib::Math::Euler &other) { rotate_ = other; }
		inline operator SoLib::Math::Euler &() { return rotate_; }
		inline operator const SoLib::Math::Euler &() const { return rotate_; }
	};

	struct TransformMatComp : IComponent {
		Matrix4x4 transformMat_;
		inline Matrix4x4 &operator=(const Matrix4x4 &other) { transformMat_ = other; }
		inline operator Matrix4x4 &() { return transformMat_; }
		inline operator const Matrix4x4 &() const { return transformMat_; }
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