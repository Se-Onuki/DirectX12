#pragma once
#include <tuple>
#include <memory>
#include <array>
#include <list>
#include <vector>
#include "../World/ComponentArray/ComponentData.h"
#include "../Component/Component.hpp"
#include "../Component/ComponentRegistry.h"
#include "../World/NewWorld.h"

namespace ECS {

	class Chunk;

	struct IFunctionalSystem {
		virtual ~IFunctionalSystem() = default;
		virtual void Execute() = 0;
		float deltaTime_ = 0.f;
	};

	/*template<typename... Ts>
	using GetCompReadOnly = std::tuple<ComponentData::TRange<Ts>...>;
	template<typename... Ts>
	using GetCompWriteOnly = std::tuple<ComponentData::TRange<Ts>...>;*/
	template<typename... Ts>
	struct GetComponentHelper {
		static constexpr std::array<uint32_t, sizeof...(Ts)> kTarget_{ static_cast<uint32_t>(ECS::ComponentRegistry::GetIndex<Ts>())... };
		using Source = std::tuple<ComponentData::TRange<Ts>...>;
		using Components = std::tuple<Ts*...>;

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
	};

	struct TestSystem : public IFunctionalSystem {

		using ReadWrite = GetComponentHelper<ECS::PositionComp, ECS::TransformMatComp>;
		TestSystem() = default;
		TestSystem(std::byte **ptr) { ReadWrite::MemCpy(readWrite_, ptr); }

		ReadWrite::Components readWrite_;

		void Execute() override {
			auto &[pos, mat] = readWrite_;

			*mat = Matrix4x4::Identity();
			mat->transformMat_.GetTranslate() = *pos;
		}


	};

	class SystemExecuter {
	public:

		using SystemData = std::pair<std::span<const uint32_t>, std::unique_ptr<IFunctionalSystem>(*)(std::byte **)>;

		SystemExecuter() = default;

		template<SoLib::IsBased<IFunctionalSystem> T>
		void AddSystem();

		void Execute(const SystemData &systemData, Chunk *chunk);
		void Execute(Chunk *chunk);
		void Execute(World *world);

		std::list<SystemData> systems_;

	private:

	};

	template<SoLib::IsBased<IFunctionalSystem> T>
	inline void SystemExecuter::AddSystem()
	{
		// データの参照元と､実行データの構築関数を保存する
		systems_.push_back({ std::span<const uint32_t>{ T::ReadWrite::kTarget_.data(), T::ReadWrite::kTarget_.size() }, [](std::byte **ptr)->std::unique_ptr<IFunctionalSystem> { return std::make_unique<T>(ptr); } });
	}

}