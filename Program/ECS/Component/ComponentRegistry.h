#pragma once
#include <tuple>
#include <bitset>
#include <cstddef>
#include <iostream>

#include "../../Utils/SoLib/SoLib_Traits.h"
#include "../../Utils/Containers/Singleton.h"
#include "Component.hpp"

namespace ECS {

	// 型の情報
	struct TypeData {
		template <IsComponent T>
		inline static TypeData CreateTypeData() {
			TypeData result{
				.typeIndex_ = typeid(T),
				.typeSize_ = sizeof(T),
				.typeAlignas_ = alignof(T)
			};
			return result;
		}


		// 型情報
		std::type_index typeIndex_;
		// 型のサイズ
		size_t typeSize_;
		// 型のアライメントサイズ
		size_t typeAlignas_;



	};

	template <IsComponent... TComps>
	class BaseComponentRegistry : public SoLib::Singleton<BaseComponentRegistry<TComps...>> {
		friend SoLib::Singleton<BaseComponentRegistry<TComps...>>;
	public:


	public:

		using kTypes_ = std::tuple<TComps...>;
		std::array<TypeData, sizeof...(TComps)> typeDatas_{ TypeData::CreateTypeData<TComps>()... };

		class ComponentFlag {
		public:
			friend class DebuggerVisualizer;
			using BitData = std::bitset<sizeof...(TComps)>;

		private:
			// メンバ変数としてのbitset
			BitData bitset_;

		public:

			// Archetypeのコンストラクタ
			ComponentFlag() : bitset_{} {}

			uint32_t GetCompCount() const { return static_cast<uint32_t>(bitset_.count()); }

			BitData Get() const {
				return bitset_;
			}

			template <IsComponent T, IsComponent... Ts>
			void AddComp() {
				constexpr std::size_t index = GetIndex<T, TComps...>();
				bitset_.set(index);

				if constexpr (sizeof...(Ts)) {
					AddComp<Ts...>();
				}

			}

			// 型に対応するビットをリセットする関数
			template <IsComponent T, IsComponent... Ts>
			void RemoveComp() {
				constexpr std::size_t index = GetIndex<T, TComps...>();
				bitset_.reset(index);
				if constexpr (sizeof...(Ts)) {
					RemoveComp<Ts...>();
				}
			}

			// 型に対応するビットがセットされているかを確認する関数
			template <IsComponent T>
			bool IsHasComp() const {
				constexpr std::size_t index = GetIndex<T, TComps...>();
				return bitset_.test(index);
			}



		private:

			// 型Tのインデックスを取得するメタ関数
			template <IsComponent T, IsComponent... Types>
			struct TypeIndex;

			template <IsComponent T, IsComponent First, IsComponent... Rest>
			struct TypeIndex<T, First, Rest...> {
				static constexpr std::size_t value = std::is_same_v<T, First> ? 0 : 1 + TypeIndex<T, Rest...>::value;
			};

			template <IsComponent T>
			struct TypeIndex<T> {
				static constexpr std::size_t value = 0;
			};

			template <IsComponent T, IsComponent... Types>
			static constexpr std::size_t GetIndex() {
				return TypeIndex<T, Types...>::value;
			}
		};

		static ComponentFlag CreateFlag() {
			return ComponentFlag{};
		}

		template <IsComponent... TComps>
		static ComponentFlag CreateFlag() {
			ComponentFlag result{};
			result.AddComp<TComps...>();
			return result;
		}
	};




}


template <ECS::IsComponent... TComps>
inline static constexpr ECS::BaseComponentRegistry<TComps...>::ComponentFlag::BitData operator&(const typename ECS::BaseComponentRegistry<TComps...>::ComponentFlag::BitData &l, const typename ECS::BaseComponentRegistry<TComps...>::ComponentFlag::BitData &r) noexcept {
	return l & r;
}

template <ECS::IsComponent... TComps>
inline static constexpr ECS::BaseComponentRegistry<TComps...>::ComponentFlag::BitData operator|(const typename ECS::BaseComponentRegistry<TComps...>::ComponentFlag::BitData &l, const typename ECS::BaseComponentRegistry<TComps...>::ComponentFlag::BitData &r) noexcept {
	return l | r;
}

namespace ECS {
	using ComponentRegistry = BaseComponentRegistry<IsAlive, AccelerationComp, GravityComp, PlayerTag, IsLanding, EnemyTag, ParticleComp, EmitterComp, ColorLarp, Identifier, BillboardRotate, AliveTime, LifeLimit, Color, PositionComp, VelocityComp, ScaleComp, RotateComp, QuaternionRotComp, TransformMatComp, ModelComp, FollowCamera, BoneTransformComp, EntityState, ModelAnimator, SkinModel, AttackCollisionComp, SphereCollisionComp, OBBCollisionComp, HealthComp, HealthBarComp, InvincibleTime, AttackPower, AttackCooltime, AirResistance, CursorComp, AttackStatus, Parent, CreateByLevelData, InputFlagComp>;
}