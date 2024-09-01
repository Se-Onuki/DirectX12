#pragma once
#include <tuple>
#include <memory>
#include <array>
#include <list>
#include <vector>
#include "../World/ComponentArray/ComponentData.h"
#include "../Component/Component.hpp"
#include "../Component/ComponentRegistry.h"
#include "../Archetype.h"
#include "../../Utils/SoLib/SoLib_Traits.h"

namespace ECS {

	class Chunk;
	class World;

	struct IJobEntity {};

	struct IFunctionalSystem {
		virtual ~IFunctionalSystem() = default;
		virtual void PreExecute() {}
		virtual void Execute() = 0;
		virtual void AfterExecute() {}

		inline static World *world_;

		float deltaTime_ = 0.f;
	};


	template<typename... Ts>
	struct GetComponentHelper {
		using Types = std::tuple<Ts...>;
		static constexpr std::array<uint32_t, sizeof...(Ts)> kTarget_{ static_cast<uint32_t>(ECS::ComponentRegistry::GetIndex<Ts>())... };
		using Source = std::tuple<ComponentData::TRange<Ts>...>;
		template<bool IsConst>
		using ComponentsBase = std::conditional_t<IsConst, std::tuple<const Ts&...>, std::tuple<Ts&...>>;
		using Components = ComponentsBase<false>;
		using ConstComponents = ComponentsBase<true>;

		static Archetype GetArchetype() {
			Archetype archetype;
			archetype.AddClassData<Ts...>();
			return archetype;
		}

		static Archetype &SetClassData(Archetype &archetype) {

			archetype.AddClassData<Ts...>();

			return archetype;
		}

		static std::byte **MemCpy(Components &comps, std::byte **src) {

			uint32_t index = 0u;
			(
				(
					[&]()
					{
						std::memcpy(&std::get<Ts *>(comps), &(src[index]), sizeof(size_t));
						index++;
					}()
						), ...
				);
			return src;
		}
		template<bool IsConst = false>
		static ComponentsBase<IsConst> Copy(std::byte **&src) {

			std::tuple<Ts*...> result{};
			uint32_t index = 0u;
			(
				(
					[&]()
					{
						std::memcpy(&std::get<Ts *>(result), &(src[index]), sizeof(size_t));
						index++;
					}()
						), ...
				);
			src = &src[index];
			return *reinterpret_cast<ComponentsBase<IsConst> *>(&result);

		}

	};

	namespace ComponentAccessor {

		/// @brief コンポーネントのデータの取得のインターフェイス
		struct IGetComponent {};

		/// @brief 読み込み限定のアクセッサ
		template <typename... Ts>
			requires (sizeof...(Ts) > 0)
		struct OnlyRead : IGetComponent {
			static constexpr uint32_t kCompCount_ = static_cast<uint32_t>(sizeof...(Ts));
			using ComponentList = GetComponentHelper<Ts...>;
			ComponentList::ConstComponents onlyRead_;
			static constexpr bool kIsConst_ = true;
			OnlyRead() = default;
			OnlyRead(const OnlyRead &) = default;
			OnlyRead(OnlyRead &&) = default;
			OnlyRead(const ComponentList::Components &arg) :onlyRead_(arg) {};
			OnlyRead(ComponentList::Components &&arg) :onlyRead_(std::move(arg)) {};
			OnlyRead(const ComponentList::ConstComponents &arg) :onlyRead_(arg) {};
			OnlyRead(ComponentList::ConstComponents &&arg) :onlyRead_(std::move(arg)) {};
		};

		/// @brief 書き込み限定のアクセッサ
		template <typename... Ts>
			requires (sizeof...(Ts) > 0)
		struct OnlyWrite : IGetComponent {
			static constexpr uint32_t kCompCount_ = static_cast<uint32_t>(sizeof...(Ts));
			using ComponentList = GetComponentHelper<Ts...>;
			ComponentList::Components onlyWrite_;
			static constexpr bool kIsConst_ = false;
			OnlyWrite() = default;
			OnlyWrite(const ComponentList::Components &arg) :onlyWrite_(arg) {};
		};

		/// @brief 読み書き可能のアクセッサ
		template <typename... Ts>
			requires (sizeof...(Ts) > 0)
		struct ReadAndWrite : IGetComponent {
			static constexpr uint32_t kCompCount_ = static_cast<uint32_t>(sizeof...(Ts));
			using ComponentList = GetComponentHelper<Ts...>;
			ComponentList::Components readAndWrite_;
			static constexpr bool kIsConst_ = false;
			ReadAndWrite() = default;
			ReadAndWrite(const ComponentList::Components &arg) :readAndWrite_(arg) {};
		};

		/// @brief 除外設定
		template <typename... Ts>
			requires (sizeof...(Ts) > 0)
		struct Exclusions : IGetComponent {
			static constexpr uint32_t kCompCount_ = static_cast<uint32_t>(sizeof...(Ts));
			using ComponentList = GetComponentHelper<Ts...>;
		};

		template <typename Type, std::size_t... sizes>
		constexpr auto concatenate(const std::array<Type, sizes>&... arrays)
		{
			std::array<Type, (sizes + ...)> result;
			std::size_t index{};

			((std::copy_n(arrays.begin(), sizes, result.begin() + index), index += sizes), ...);

			return result;
		}

		template <typename Type, std::size_t size>
		constexpr auto reverse(const std::array<Type, size> &arr)
		{
			std::array<Type, size> result = arr;
			std::reverse(result.begin(), result.end());
			return result;
		}



		template<SoLib::IsBased<IGetComponent>... Ts>
		struct DataBase {
			static constexpr std::array<std::span<const uint32_t>, sizeof...(Ts)> compRanges_ = { {std::span<const uint32_t>{ Ts::ComponentList::kTarget_.data(), Ts::ComponentList::kTarget_.size() }...} };

			inline static constexpr auto kTarget_ = concatenate(reverse(Ts::ComponentList::kTarget_)...);

			inline static constexpr ECS::ComponentRegistry::ComponentFlag kExclusions_{};

		};

		struct CompStorageHelper {
			constexpr static uint32_t kMaxAccessor_ = 3u;

			// コンポーネントの取得元
			std::array<std::span<const uint32_t>, kMaxAccessor_> ranges_;
			uint32_t size_ = 0u;

			Archetype archetype_;

			/// @brief アクセッサの追加
			/// @tparam T アクセッサの型
			template<SoLib::IsBased<IGetComponent> T>
			void AddAccessor() {
				if (size_ >= kMaxAccessor_) { return; }
				// 追加する型の保存先
				auto &range = ranges_[size_++];	// 保存先を加算して追加する
				// 型に含まれるコンポーネントのデータの配列
				range = { T::ComponentList::kTarget_.data(), T::ComponentList::kTarget_.size() };

				archetype_.AddClassData(T::ComponentList::kTarget_);
			}
		};
	}

	/// @brief 書き込み先のデータ
	template<typename...Ts>
	struct OutPutData {
		std::tuple<Ts...> target_;

	};

}


namespace std {
	// std::tuple_size の特殊化
	template<typename... Ts>
	struct tuple_size<ECS::ComponentAccessor::OnlyRead<Ts...>> : std::integral_constant<std::size_t, sizeof...(Ts)> {};
	template<typename... Ts>
	struct tuple_size<ECS::ComponentAccessor::OnlyWrite<Ts...>> : std::integral_constant<std::size_t, sizeof...(Ts)> {};
	template<typename... Ts>
	struct tuple_size<ECS::ComponentAccessor::ReadAndWrite<Ts...>> : std::integral_constant<std::size_t, sizeof...(Ts)> {};


	// std::tuple_element の特殊化
	template<std::size_t Index, typename... Ts>
	struct tuple_element<Index, ECS::ComponentAccessor::OnlyRead<Ts...>> {
		using type = std::tuple_element_t<Index, decltype(ECS::ComponentAccessor::OnlyRead<Ts...>::onlyRead_)>;
	};

	// std::tuple_element の特殊化
	template<std::size_t Index, typename... Ts>
	struct tuple_element<Index, ECS::ComponentAccessor::OnlyWrite<Ts...>> {
		using type = std::tuple_element_t<Index, decltype(ECS::ComponentAccessor::OnlyWrite<Ts...>::onlyWrite_)>;
	};

	// std::tuple_element の特殊化
	template<std::size_t Index, typename... Ts>
	struct tuple_element<Index, ECS::ComponentAccessor::ReadAndWrite<Ts...>> {
		using type = std::tuple_element_t<Index, decltype(ECS::ComponentAccessor::ReadAndWrite<Ts...>::readAndWrite_)>;
	};
}

namespace ECS::ComponentAccessor {

	// get 関数の定義
	template<std::size_t Index, typename... Ts>
	decltype(auto) get(const OnlyRead<Ts...> &obj) {
		return std::get<Index>(obj.onlyRead_);
	}
	// get 関数の定義
	template<std::size_t Index, typename... Ts>
	decltype(auto) get(const OnlyWrite<Ts...> &obj) {
		return std::get<Index>(obj.onlyWrite_);
	}
	// get 関数の定義
	template<std::size_t Index, typename... Ts>
	decltype(auto) get(const ReadAndWrite<Ts...> &obj) {
		return std::get<Index>(obj.readAndWrite_);
	}
}


namespace ECS {

	using namespace ECS::ComponentAccessor;

	//struct NewTestSystem : public IFunctionalSystem {
	//	OnlyRead<ECS::ScaleComp, ECS::QuaternionRotComp, ECS::PositionComp> onlyRead_;

	//	OnlyWrite<ECS::TransformMatComp> onlyWrite_;

	//	using DataBase = DataBase<NewTestSystem, decltype(onlyRead_), decltype(onlyWrite_)>;
	//	NewTestSystem(const decltype(onlyRead_)::ComponentList::Components &onlyRead, const decltype(onlyWrite_)::ComponentList::Components &onlyWrite) :onlyRead_(onlyRead), onlyWrite_(onlyWrite) {}

	//	void Execute() override {
	//		const auto &[scale, rot, pos] = onlyRead_;
	//		auto &[transMat] = onlyWrite_;

	//		transMat = SoLib::Affine(scale, rot.quateRot_.Normalize(), pos);
	//	}


	//};

	struct StructBaseSystem : IJobEntity {
		OnlyRead<ECS::ScaleComp, ECS::QuaternionRotComp, ECS::PositionComp> onlyRead_;
		OnlyWrite<ECS::TransformMatComp> onlyWrite_;

		using DataBase = DataBase<decltype(onlyRead_), decltype(onlyWrite_)>;

		void Execute(const World *const, const float) {

			const auto &[scale, rot, pos] = onlyRead_;
			auto &[transMat] = onlyWrite_;

			transMat = SoLib::Affine(scale, rot.quateRot_.Normalize(), pos);
		}

	};

	struct TestSystem : public IFunctionalSystem {

		using ReadWrite = GetComponentHelper<ECS::PositionComp, ECS::TransformMatComp>;
		TestSystem() = default;
		TestSystem(std::byte **ptr) : readWrite_(ReadWrite::Copy(ptr)) {}

		ReadWrite::Components readWrite_;

		void Execute() override {
			auto &[pos, mat] = readWrite_;

			mat = Matrix4x4::Identity();
			mat.transformMat_.GetTranslate() = pos;
		}

	};

	class SystemExecuter {
	public:

		struct SystemData {
			Archetype archetype_;
			// 除外設定
			ECS::ComponentRegistry::ComponentFlag exclusions_;
			std::span<const uint32_t> keys_;
			void (*executor_)(IJobEntity *, const World *, float);
			bool isSingleThreads_;
		};

		SystemExecuter() = default;

		template<SoLib::IsBased<IJobEntity> T>
		void AddSystem();

		/*template<SoLib::IsBased<IJobEntity> T>
		void AddNewSystem();*/

		void Execute(const SystemData &systemData, const World *const world, Chunk *chunk, float deltaTime);
		void Execute(World *world, float deltaTime);

		std::list<SystemData> systems_;

	private:

	};


	template <typename T>
	concept HasSingleThreadFlag = requires { { T::kIsSingleThread_ } -> std::convertible_to<bool>; };

	template <typename T>
	constexpr bool IsSingleThread() {
		if constexpr (HasSingleThreadFlag<T>) {
			return true;
		}
		else {
			return false;
		}
	}

	template <typename T>
	concept HasExclusion = requires { { T::Exclusions::kCompCount_ } -> std::convertible_to<uint32_t>; };

	template <typename T>
	constexpr bool IsHasExclusion() {
		if constexpr (HasExclusion<T>) {
			return true;
		}
		else {
			return false;
		}
	}

	template<SoLib::IsBased<IJobEntity> T>
	inline void SystemExecuter::AddSystem()
	{
		Archetype archetype;
		archetype.AddClassData(T::DataBase::kTarget_);
		ECS::ComponentRegistry::ComponentFlag exclusions;
		if constexpr (IsHasExclusion<T>()) { exclusions = T::Exclusions::ComponentList::GetArchetype().compFlag_; }

		// データの参照元と､実行データの構築関数を保存する
		systems_.push_back({ archetype, exclusions, std::span<const uint32_t>{ T::DataBase::kTarget_.data(), T::DataBase::kTarget_.size() },[](IJobEntity *const iSystem, const World *const world, const float deltaTime) { static_cast<T *>(iSystem)->Execute(world, deltaTime); }, IsSingleThread<T>() });
	}

}