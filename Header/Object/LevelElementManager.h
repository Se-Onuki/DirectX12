#pragma once
#include <list>
#include <memory>

#include "../../Engine/DirectBase/Render/Camera.h"
#include "../Collision/Collision.h"
#include "../../Engine/DirectBase/Model/Model.h"

#include "../../Engine/DirectBase/File/VariantItem.h"
#include "../../Utils/SoLib/SoLib_Timer.h"
#include <array>
#include <map>

class LevelElementManager {
	LevelElementManager() = default;
	LevelElementManager(const LevelElementManager &) = delete;
	LevelElementManager &operator=(const LevelElementManager &) = delete;
	~LevelElementManager() = default;
public:

	enum class GroundType {
		kGrass,
		kDirt,

		kSize
	};

	class Box {
	public:
		Box(const AABB &aabb);

		GroundType groundType_ = GroundType::kGrass;

		Transform transform_;
		AABB referenceBox_;

		bool ImGuiWidget();
	};

	class Platform {
	public:
		Platform() = default;
		~Platform() = default;

		BaseTransform center_;
		Vector3 startRot_;
		Vector3 targetRot_;

		Vector3 rotateAxis_ = Vector3::front;

		VariantItem<float> vLerpTime_{ "LerpTime",1.f };

		void AddBox(const AABB &box);
		void CalcCollision();

		void Update(float deltaTime);

		void AddRotate(const float targetRot);

		void SetAxis(const Vector3 &axis) { rotateAxis_ = axis.Nomalize(); }

		void Draw(const Camera3D &camera) const;

		const auto &GetCollider() const { return collisionBox_; }

		void ImGuiWidget();

	private:



		SoLib::DeltaTimer timer_;
		std::list<AABB> collisionBox_;

		std::list<Box> boxList_;
	};

public:

	static auto *const GetInstance() {
		static LevelElementManager instance{};
		return &instance;
	}

	void ImGuiWidget();

	void Init();

	void Update(float deltaTime);

	void Draw(const Camera3D &camera) const;

	void DebugDraw(const Camera3D &camera) const;

	void CalcCollision(const uint32_t key);
	void CalcCollision();

	/// @brief ブロックを追加
	/// @param transform ブロックのSRT
	void AddBlock(const uint32_t key, const AABB &box);

	const LineBase &GetStageLine() const { return stageLine_; }

	Platform *const GetPlatform(int32_t index);

	using PlatformMap = std::map<uint32_t, Platform>;
	PlatformMap blockCollider_;

	const auto &GetGroundModel() const { return groundModels_; }

private:
	std::array<Model *, 2u> groundModels_;
	Transform lineStart_;
	Transform lineEnd_;
	LineBase stageLine_;

	bool debugDrawer_ = false;
};