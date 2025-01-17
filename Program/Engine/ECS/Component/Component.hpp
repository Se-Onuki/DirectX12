/// @file Component.h
/// @brief ECSで使用するコンポーネントの記述
/// @author ONUKI seiya
#pragma once

#include <memory>

#include <concepts>
#include <typeinfo>

#include "../../Engine/DirectBase/Model/Model.h"
#include "../../Engine/DirectBase/Model/ModelManager.h"
#include "../../Engine/Utils/Text/ConstString.h"
#include "../../Engine/Utils/Containers/ConstVector.h"
#include "../../Engine/Utils/Graphics/Color.h"
#include "../../Engine/Utils/SoLib/SoLib.h"
#include "../../Engine/Utils/Math/Euler.h"
#include "../../Engine/Utils/Math/Transform.h"
#include "../../Engine/Utils/Math/Vector2.h"
#include "../../Engine/Utils/Math/Vector3.h"
#include "../../Engine/Utils/Math/Polar.h"
#include <bitset>
#include <string>
#include <unordered_map>
#include <typeindex>

#include "../../Engine/DirectBase/Model/BoneModel.h"
#include "../../Header/Collision/Collision.h"
#include "../../Engine/LevelEditor/LevelData.h"
#include "../../Engine/DirectBase/Model/SkeletonAnimation/ModelAnimation.h"
#include "../../Engine/DirectBase/SkinAnimation/SkinAnimation.h"

namespace ECS {

	/// @brief コンポーネントであるというフラグ
	struct IComponent {};

	/// @brief 共有コンポーネントであるというフラグ
	struct ISharedComponent {};

	template <size_t Size>
	struct ArrayComponent : IComponent {
		inline static constexpr size_t kSize_ = Size;
	};

	struct IsAlive : IComponent {
		bool isAlive_ = true;

		inline bool operator==(const IsAlive &r) const { return isAlive_ == r.isAlive_; }
	};

	struct AccelerationComp : IComponent {
		Vector3 acceleration_;
	};

	struct GravityComp : IComponent {
		Vector3 gravity_ = Vector3::up * -9.8f;
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
		int32_t count_{};
		SoLib::DeltaTimer frequency_;
		SoLib::Color::RGB4 startColor_ = 0xFFFFFFFF;
		SoLib::Color::RGB4 endColor_{};

		ValuePair<float> spawnLifeLimit_{ };

		ValuePair<float> spawnPower_{};
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
		SoLib::Color::RGB4 start_{};
		SoLib::Color::RGB4 end_{};
		SoLib::EaseFunc easeFunc_{};

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

		inline Vector3 &operator*() noexcept { return position_; }
		inline const Vector3 &operator*() const noexcept { return position_; }
	};

	struct VelocityComp : IComponent {
		Vector3 velocity_;
		inline Vector3 &operator=(const Vector3 &other) { return velocity_ = other; }
		inline operator Vector3 &() { return velocity_; }
		inline operator const Vector3 &() const { return velocity_; }

		inline Vector3 &operator*() noexcept { return velocity_; }
		inline const Vector3 &operator*() const noexcept { return velocity_; }
	};

	struct ScaleComp : IComponent {
		Vector3 scale_ = Vector3::one;
		inline Vector3 &operator=(const Vector3 &other) { return scale_ = other; }
		inline operator Vector3 &() { return scale_; }
		inline operator const Vector3 &() const { return scale_; }

		inline Vector3 &operator*() noexcept { return scale_; }
		inline const Vector3 &operator*() const noexcept { return scale_; }
	};

	struct RotateComp : IComponent {
		SoLib::Math::Euler rotate_;
		inline SoLib::Math::Euler &operator=(const SoLib::Math::Euler &other) { return rotate_ = other; }
		inline operator SoLib::Math::Euler &() { return rotate_; }
		inline operator const SoLib::Math::Euler &() const { return rotate_; }
	};

	struct QuaternionRotComp : IComponent {
		Quaternion quateRot_ = Quaternion::Identity;
		inline Quaternion &operator=(const Quaternion &other) { return quateRot_ = other; }
		inline operator Quaternion &() { return quateRot_; }
		inline operator const Quaternion &() const { return quateRot_; }
	};

	struct TransformMatComp : IComponent {
		Matrix4x4 transformMat_;
		inline Matrix4x4 &operator=(const Matrix4x4 &other) { return transformMat_ = other; }
		inline operator Matrix4x4 &() { return transformMat_; }
		inline operator const Matrix4x4 &() const { return transformMat_; }
	};

	struct ModelComp : IComponent, ISharedComponent {

		SolEngine::ResourceHandle<SolEngine::ModelData> model_;

	};

	struct GhostModel : IComponent {};

	struct UnRender : IComponent {};

	struct HasShadow : IComponent {};

	struct ExpOrb : IComponent {};

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

	struct EntityState : IComponent {
		enum class PlayerState : uint32_t {
			kIdle,
			kWalk,
			kAttack,
		};

		// 遷移状態の情報
		uint32_t stateIndex_{};

		// 状態の
		float stateTimer_ = 0.f;
		/// @brief 状態を変更する
		/// @param[in] nextState 変更したい状態
		void ChangeState(uint32_t nextState);
	};

	struct ModelAnimator : IComponent {

		std::array<SolEngine::ResourceHandle<SolEngine::Animation>, 4u> animateList_;

		SolEngine::AnimationPlayer animatior_;
	};

	// スキンモデルコンポーネント
	struct SkinModel : IComponent {
		// スキンモデルのポインタ
		::SkinModel *skinModel_;
	};

	// 攻撃のコリジョンのコンポーネント
	struct AttackCollisionComp : IComponent {
		// 球体
		Sphere collision_{};
		// 動作中か
		bool isActive_ = false;
	};

	// 球のコリジョン
	struct SphereCollisionComp : IComponent {
		Sphere collision_;
	};

	// Obbのコリジョン
	struct OBBCollisionComp : IComponent {
		OBB collision_;
	};

	// 攻撃のベクトルコンポーネント
	struct AttackFacing : IComponent {
		Vector3 facing_{};
	};

	// 体力コンポーネント
	struct HealthComp : IComponent {
		int32_t maxHealth_ = 0;
		int32_t nowHealth_ = 0;

		/// @brief 体力の回復
		void Reset() noexcept { nowHealth_ = maxHealth_; }
		/// @brief 体力の割合
		float CalcPercent() const noexcept { return nowHealth_ < 0 ? 0.f : static_cast<float>(nowHealth_) / maxHealth_; }
		/// @brief 死んでいるか
		bool IsDead() const noexcept { return nowHealth_ <= 0; }
		/// @brief 最大値か
		bool IsMax() const noexcept { return maxHealth_ == nowHealth_; }

		/// @brief 生成
		/// @param[in] health 体力
		static HealthComp Create(int32_t health) { return HealthComp{ .maxHealth_ = health, .nowHealth_ = health }; }
	};

	struct HealthBarComp : IComponent {
		Vector3 offset_{ .y = 2.f };
		VariantItem<"DefaultBarScale", Vector2> vDefaultBarScale_{ {360.f,32.f * 1.5f} };
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

	struct CursorComp : IComponent {
		float offset_{ 5.f };
		float scale_{ 2.f };
		float progress_ = 0.f;
		Model *model_ = nullptr;
		Model *inModel_ = nullptr;
	};

	struct AttackStatus : IComponent {
		float offset_ = 0.f;
		float radius_ = 4.5f;
	};
	struct Parent : IComponent {
	};

	struct Experience : IComponent {
		uint32_t level_{};
		uint32_t exp_{};
		uint32_t(*needExp_)(uint32_t level) = [](uint32_t level)->uint32_t { return 10 + static_cast<uint32_t>(0.25f * std::powf(static_cast<float>(level), 1.5f)); };
	};

	/// @brief レベルデータから構築されたという証明書
	struct CreateByLevelData : IComponent {
		SolEngine::ResourceHandle<SolEngine::LevelData> handle_;
	};

	// 範囲攻撃の演出
	struct AttackRangeCircle : IComponent {};
	// 矢の攻撃の演出
	struct AttackArrow : IComponent {};

	// 物理挙動
	struct Rigidbody : IComponent {};

	// 外部に影響を受けない物理挙動
	struct KinematicRigidbody : IComponent {};

	/// @brief 当たったら押し出す方向｡
	struct KnockBackDirection : IComponent {
		// 当たった対象がベクトルに対して押し出される方向｡範囲内乱数のローカルの弧度法｡
		// もしZeroなら位置基準の相対方向
		Vector2 diff_{};
		// 押し出す強さ
		ValuePair<float> diffPower_{};
	};

	struct EffectPlayer : IComponent {

	};

	struct InputFlagComp : IComponent {};

	//template<SoLib::IsBased<IComponent> T, uint32_t size>
	//struct ComponentArray :IComponent, public std::array<T, size> {	};

	template<class T>
	concept IsComponent = SoLib::IsBased<T, IComponent>;
}

namespace std {
	template<>
	struct hash<ECS::ModelComp> {
		size_t operator()(const ECS::ModelComp data) const {
			return data.model_.GetHashID();
		}
	};
}