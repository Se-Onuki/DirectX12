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
				.typeAlignas_ = alignof(T),
				.constructor_ = [](void *ptr) { new(ptr) T{}; }
			};
			return result;
		}


		// 型情報
		std::type_index typeIndex_;
		// 型のサイズ
		uint32_t typeSize_;
		// 型のアライメントサイズ
		uint32_t typeAlignas_;
		// デフォルトコンストラクタ
		void (*constructor_)(void *) = nullptr;


	};

	template <IsComponent... TComps>
	class BaseComponentRegistry : public SoLib::Singleton<BaseComponentRegistry<TComps...>> {
		friend SoLib::Singleton<BaseComponentRegistry<TComps...>>;
		using Singleton = SoLib::Singleton<BaseComponentRegistry<TComps...>>;
	public:


	public:
		inline static constexpr uint32_t kSize = sizeof...(TComps);

		using kTypes_ = std::tuple<TComps...>;
		std::array<TypeData, kSize> typeDatas_{ TypeData::CreateTypeData<TComps>()... };

		class ComponentFlag {
		public:
			using BitData = std::bitset<sizeof...(TComps)>;

			inline static constexpr uint32_t kSize = sizeof...(TComps);
			/*friend static ComponentFlag BaseComponentRegistry<TComps...>::operator&(const ComponentFlag &, const ComponentFlag &) noexcept;
			friend static ComponentFlag BaseComponentRegistry<TComps...>::operator|(const ComponentFlag &, const ComponentFlag &) noexcept;*/


		private:
			// メンバ変数としてのbitset
			BitData bitset_;

		public:

			// Archetypeのコンストラクタ
			ComponentFlag() = default;
			ComponentFlag(BitData &&bitset) :bitset_(bitset) {}

			uint32_t GetCompCount() const { return static_cast<uint32_t>(bitset_.count()); }

			operator const BitData() { return bitset_; }
			operator const BitData &() const { return bitset_; }

			const BitData &Get() const {
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

			constexpr bool operator==(const ComponentFlag &) const noexcept = default;


			ComponentFlag operator&(const ComponentFlag &r) noexcept {
				return { bitset_ & r.bitset_ };
			}

			ComponentFlag operator|(const ComponentFlag &r) noexcept {
				return { bitset_ | r.bitset_ };
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

namespace ECS {

	//template <IsComponent... TComps>
	//const typename BaseComponentRegistry<TComps...>::ComponentFlag &operator&(const typename BaseComponentRegistry<TComps...>::ComponentFlag &l, const typename BaseComponentRegistry< TComps...>::ComponentFlag &r) {
	//	return { l.Get() & r.Get() };
	//}

	//template <ECS::IsComponent... TComps>
	//ECS::BaseComponentRegistry<TComps...>::ComponentFlag operator|(const class ECS::BaseComponentRegistry<TComps...>::ComponentFlag &l, const class ECS::BaseComponentRegistry<TComps...>::ComponentFlag &r) noexcept {
	//	return typename BaseComponentRegistry<TComps...>::ComponentFlag{ l.bitset_ | r.bitset_ };
	//}


	using ComponentRegistry = BaseComponentRegistry<IsAlive, AccelerationComp, GravityComp, PlayerTag, IsLanding, EnemyTag, ParticleComp, EmitterComp, ColorLarp, Identifier, BillboardRotate, AliveTime, LifeLimit, Color, PositionComp, VelocityComp, ScaleComp, RotateComp, QuaternionRotComp, TransformMatComp, ModelComp, FollowCamera, BoneTransformComp, EntityState, ModelAnimator, SkinModel, AttackCollisionComp, SphereCollisionComp, OBBCollisionComp, HealthComp, HealthBarComp, InvincibleTime, AttackPower, AttackCooltime, AirResistance, CursorComp, AttackStatus, Parent, CreateByLevelData, InputFlagComp>;
}
