#pragma once
#include <tuple>
#include <bitset>
#include <cstddef>
#include <iostream>

#include "../../Engine/Utils/SoLib/SoLib_Traits.h"
#include "../../Engine/Utils/Containers/Singleton.h"
#include "Component.hpp"

#define ComponentRegistry()

namespace ECS {

	// 型の情報
	struct TypeData {
		template <IsComponent T>
		inline static TypeData CreateTypeData() {
			TypeData result{
				.typeIndex_ = typeid(T),
				.typeSize_ = sizeof(T),
				.typeAlignas_ = alignof(T),
				.constructor_ = [](void *ptr) { new(ptr) T{}; },
				.isEmptyClass_ = std::is_empty_v<T>
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
		// 空の型か
		bool isEmptyClass_;

	};

	template <typename Tuple>
	class BaseComponentRegistry;

	template <IsComponent... TComps>
	class BaseComponentRegistry<std::tuple<TComps...>> : public SoLib::Singleton<BaseComponentRegistry<std::tuple<TComps...>>> {
		friend SoLib::Singleton<BaseComponentRegistry<std::tuple<TComps...>>>;
		using Singleton = SoLib::Singleton<BaseComponentRegistry<std::tuple<TComps...>>>;
	public:


	public:
		inline static constexpr uint32_t kSize = sizeof...(TComps);

		using kTypes = std::tuple<TComps...>;
		const std::array<TypeData, kSize> typeDatas_{ TypeData::CreateTypeData<TComps>()... };


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

			void AddComp(const std::initializer_list<uint32_t> &args) {
				for (uint32_t arg : args) {
					bitset_.set(arg);
				}
			}

			template<SoLib::IsContainsType<uint32_t> T>
			void AddComp(const T &args) {
				for (uint32_t arg : args) {
					bitset_.set(arg);
				}
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
				constexpr std::size_t index = GetIndex<T>();
				bitset_.reset(index);
				if constexpr (sizeof...(Ts)) {
					RemoveComp<Ts...>();
				}
			}

			// 型に対応するビットがセットされているかを確認する関数
			template <IsComponent T>
			bool IsHasComp() const {
				constexpr std::size_t index = GetIndex<T>();
				return bitset_.test(index);
			}

			constexpr bool operator==(const ComponentFlag &) const noexcept = default;

			ComponentFlag operator&(const ComponentFlag &r) noexcept {
				return { bitset_ & r.bitset_ };
			}

			ComponentFlag operator|(const ComponentFlag &r) noexcept {
				return { bitset_ | r.bitset_ };
			}

			/// @brief 保持しているコンポーネントのIndexを取得する
			/// @return 保持しているIndexの配列
			std::vector<uint32_t> GetIndexList() const {
				// 返還するデータ
				std::vector<uint32_t> result;
				// メモリの確保
				result.reserve(bitset_.count());

				// 数値に戻した値
				const size_t value = bitset_.to_ullong();

				// 最下位ビットの位置を特定
				const uint32_t lowestBitPosition = static_cast<uint32_t>(std::countr_zero(value));
				// 最上位ビットの位置を特定
				const uint32_t highestBitPosition = static_cast<uint32_t>(std::log2(value));

				// ビットが立っている最初と最後までを回して､データを保存する
				for (uint32_t i = lowestBitPosition; i <= highestBitPosition; i++) {
					if (bitset_.test(i)) {
						result.push_back(i);
					}
				}

				return result;
			}

		private:
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

	private:


		// 型Tのインデックスを取得するメタ関数
		template <IsComponent T, IsComponent... Types>
		struct TypeIndex;

		template <IsComponent T, IsComponent First, IsComponent... Rest>
		struct TypeIndex<T, First, Rest...> {
			static constexpr std::size_t value = std::is_same_v<T, First> or std::is_same_v<std::remove_const_t<T>, First> ? 0 : 1 + TypeIndex<T, Rest...>::value;
		};

		template <IsComponent T>
		struct TypeIndex<T> {
			static constexpr std::size_t value = 0;
		};

		template <IsComponent T, IsComponent... Types>
			requires(sizeof...(Types) > 0)
		static constexpr std::size_t GetIndex() {
			return TypeIndex<T, Types...>::value;
		}

	public:
		template <IsComponent T>
		static constexpr std::size_t GetIndex() {
			return GetIndex<T, TComps...>();
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


	using ComponentRegistry = BaseComponentRegistry<SoLib::SortByAlignment64<std::tuple<IsAlive, AccelerationComp, GravityComp, PlayerTag, IsLanding, EnemyTag, ParticleComp, EmitterComp, ColorLarp, Identifier, BillboardRotate, AliveTime, LifeLimit, Color, PositionComp, VelocityComp, ScaleComp, RotateComp, QuaternionRotComp, TransformMatComp, ModelComp, FollowCamera, BoneTransformComp, EntityState, ModelAnimator, SkinModel, AttackCollisionComp, SphereCollisionComp, OBBCollisionComp, HealthComp, HealthBarComp, InvincibleTime, AttackPower, AttackCooltime, AirResistance, CursorComp, AttackStatus, Parent, CreateByLevelData, InputFlagComp, Experience, GhostModel, UnRender, HasShadow, ExpOrb, AttackCircle, EffectPlayer, KnockBackDirection>>::Type>;
}
