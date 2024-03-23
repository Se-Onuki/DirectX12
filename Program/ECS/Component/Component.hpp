#pragma once
#include <memory>

#include <concepts>
#include <typeinfo>

#include "../../Engine/DirectBase/Model/Model.h"
#include "../../Engine/DirectBase/Model/ModelManager.h"
#include "../../Utils/Text/ConstString.h"
#include "../../Utils/Containers/ConstVector.h"
#include "../../Utils/Graphics/Color.h"
#include "../../Utils/SoLib/SoLib.h"
#include "../../Utils/Math/Euler.h"
#include "../../Utils/Math/Transform.h"
#include "../../Utils/Math/Vector2.h"
#include "../../Utils/Math/Vector3.h"
#include <bitset>
#include <string>
#include <unordered_map>
#include <typeindex>

#include "../../Engine/DirectBase/Model/BoneModel.h"
#include "../../Header/Collision/Collision.h"

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

	template <SoLib::IsNotPointer T>
	struct IClassData {

		static const std::type_index &GetTypeid() {
			static std::type_index typeindex{ typeid(T) };
			return typeindex;
		}

		static size_t GetSize() {
			return sizeof(T);
		}

	};

	struct IsAlive : IComponent, public IClassData<IsAlive> {
		bool isAlive_ = true;
	};

	struct AccelerationComp : IComponent, public IClassData<AccelerationComp> {
		Vector3 acceleration_;
	};

	struct GravityComp : IComponent, public IClassData<GravityComp> {
		Vector3 gravity_{};
	};

	struct PlayerTag : IComponent {
		uint32_t number_;
	};

	struct IsLanding : IComponent {
		bool isLanding_;
	};

	struct EnemyTag : IComponent {
		uint32_t number_;
	};

	struct ParticleComp : IComponent {
		SoLib::Color::RGB4 start_;
		SoLib::Color::RGB4 end_;
	};

	struct EmitterComp : IComponent {
		int32_t count_;
		SoLib::DeltaTimer frequency_;
		SoLib::Color::RGB4 startColor_ = 0xFFFFFFFF;
		SoLib::Color::RGB4 endColor_{};

		ValueRange<float> spawnLifeLimit_{ };

		ValueRange<float> spawnPower_{};
		SoLib::Math::Euler spawnRange_{};

		// SoLib::ConstVector<uint32_t, 7u> color_;


		bool ImGuiWidget(const char *const label) {
			bool isChanged = false;
			isChanged |= SoLib::ImGuiWidget((label + std::string{ " : EmittCount" }).c_str(), &count_);
			isChanged |= SoLib::ImGuiWidget((label + std::string{ " : EmittSpan" }).c_str(), &frequency_);
			isChanged |= SoLib::ImGuiWidget((label + std::string{ " : SpawnLifeLimit" }).c_str(), &spawnLifeLimit_);
			isChanged |= SoLib::ImGuiWidget((label + std::string{ " : SpawnPower" }).c_str(), &spawnPower_);
			isChanged |= SoLib::ImGuiWidget((label + std::string{ " : SpawnRange" }).c_str(), &spawnRange_);
			isChanged |= SoLib::ImGuiWidget((label + std::string{ " : StartColor" }).c_str(), &startColor_);
			isChanged |= SoLib::ImGuiWidget((label + std::string{ " : EndColor" }).c_str(), &endColor_);
			return isChanged;
		}
	};
	struct ColorLarp :IComponent {
		SoLib::Color::RGB4 start_;
		SoLib::Color::RGB4 end_;
		SoLib::EaseFunc easeFunc_;

		SoLib::Color::RGB4 EaseColor(float t);
	};

	struct Identifier : IComponent {
		SoLib::Text::ConstString<8u> name_;
	};

	struct BillboardRotate : IComponent {
		std::bitset<3u> isActive_ = 0b111;
	};

	struct AliveTime : IComponent {
		float aliveTime_{};
	};

	struct LifeLimit : IComponent {
		float lifeLimit_{};
	};

	struct Color : IComponent {
		SoLib::Color::RGB4 color_ = 0xFFFFFFFF;
	};

	struct PositionComp : IComponent {
		Vector3 position_;
		inline Vector3 &operator=(const Vector3 &other) { return  position_ = other; }
		inline operator Vector3 &() { return position_; }
		inline operator const Vector3 &() const { return position_; }
	};

	struct VelocityComp : IComponent {
		Vector3 velocity_;
		inline Vector3 &operator=(const Vector3 &other) { return velocity_ = other; }
		inline operator Vector3 &() { return velocity_; }
		inline operator const Vector3 &() const { return velocity_; }
	};

	struct ScaleComp : IComponent {
		Vector3 scale_ = Vector3::one;
		inline Vector3 &operator=(const Vector3 &other) { return scale_ = other; }
		inline operator Vector3 &() { return scale_; }
		inline operator const Vector3 &() const { return scale_; }
	};

	struct RotateComp : IComponent {
		SoLib::Math::Euler rotate_;
		inline SoLib::Math::Euler &operator=(const SoLib::Math::Euler &other) { return rotate_ = other; }
		inline operator SoLib::Math::Euler &() { return rotate_; }
		inline operator const SoLib::Math::Euler &() const { return rotate_; }
	};

	struct QuaternionRotComp : IComponent {
		Quaternion quateRot_ = Quaternion::Identity;
	};

	struct TransformMatComp : IComponent {
		Matrix4x4 transformMat_;
		inline Matrix4x4 &operator=(const Matrix4x4 &other) { return transformMat_ = other; }
		inline operator Matrix4x4 &() { return transformMat_; }
		inline operator const Matrix4x4 &() const { return transformMat_; }
	};

	struct ModelComp : IComponent {
		void Init(const std::string &key) {
			model_ = ModelManager::GetInstance()->GetModel(key);
		}
		Model *model_;

	};

	struct FollowCamera : IComponent {

#pragma region ビュー行列の設定

		Quaternion rotation_ = SoLib::MakeQuaternion({ 15._deg,0.f,0.f });

		Vector3 offset_{ 0.f, 0.f,-15.f };


#pragma endregion

#pragma region 射影行列の設定
		// 垂直方向視野角
		float fovAngleY = 45._deg;
		// ビューポートのアスペクト比
		float aspectRatio = 16.f / 9;
		// 深度限界（手前側）
		float nearZ = 0.1f;
		// 深度限界（奥側）
		float farZ = 1000.0f;
#pragma endregion

		/// @brief カメラにデータを転送する
		/// @param camera 転送先のカメラ
		/// @param translate 平行移動
		void TransferData(Camera3D &camera, const Vector3 &translate) const;
	};

	struct BoneTransformComp : IComponent {
		std::array<BoneModel::SimpleTransform, 7u> boneTransform_{};
	};

	struct AnimateParametor : IComponent {
		// アニメーションの情報
		uint32_t animIndex_{};

		// アニメーションの進行度
		SoLib::DeltaTimer timer_{};
	};

	struct WeaponComp : IComponent {
		uint32_t parentIndex_;
		Sphere collision_;
	};

	struct CollisionComp : IComponent {
		Sphere collision_;
	};

	struct HealthComp : IComponent {
		int32_t maxHealth_ = 0;
		int32_t nowHealth_ = 0;

		void Reset() { nowHealth_ = maxHealth_; }
		float CalcPercent() const { return nowHealth_ < 0 ? 0.f : static_cast<float>(nowHealth_) / maxHealth_; }
		bool IsDead() const { return nowHealth_ <= 0; }
	};

	struct InvincibleTime : IComponent {
		SoLib::DeltaTimer timer_;
	};

	struct AttackPower : IComponent {
		int32_t power_;
	};

	struct AttackCooltime : IComponent {
		SoLib::DeltaTimer cooltime_;
	};

	struct AirResistance : IComponent {
		float resistance;
	};

	struct InputFlagComp : IComponent {};

	template<class T>
	constexpr bool cIsComponentData = std::is_base_of_v<IComponent, T> &&std::is_trivially_copyable_v<T> &&std::is_trivially_destructible_v<T>;

	template<class T>
	concept BasedIComponent = cIsComponentData<T>;
}