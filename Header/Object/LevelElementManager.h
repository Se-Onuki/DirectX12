#pragma once
#include <array>
#include <iostream>
#include <json.hpp>
#include <list>
#include <map>
#include <memory>
#include <optional>

#include "../../Engine/DirectBase/File/VariantItem.h"
#include "../../Engine/DirectBase/Model/Model.h"
#include "../../Engine/DirectBase/Render/Camera.h"
#include "../../Utils/SoLib/SoLib_Timer.h"
#include "../Collision/Collision.h"
#include "../Entity/Entity.h"

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

	class Platform;

	class Box {
	public:
		Box(const AABB &aabb, Platform *parent);

		void CreateBox() const;

		GroundType groundType_ = GroundType::kGrass;

		Transform transform_;
		AABB referenceBox_;

		Platform *parent_ = nullptr;

		bool isDelete_ = false;

		bool ImGuiWidget();
	};

	class Platform {
	public:
		Platform();
		~Platform() = default;

		Transform axisBar_;

		BaseTransform center_;
		Vector3 startRot_;
		Vector3 targetRot_;

		Vector3 rotateAxis_ = Vector3::front;

		Box &AddBox(const AABB &box, GroundType groundType);

		void AddItem(const BaseTransform &srt);

		void CalcCollision();

		void Update(float deltaTime);

		void AddRotate(const float targetRot);

		void SetRotate(const float targetRot);

		void SetAxis(const Vector3 &axis) { rotateAxis_ = axis.Nomalize(); }

		void Draw(const Camera3D &camera) const;

		const auto &GetCollider() const { return collisionBox_; }

		const auto &GetBoxList() const { return boxList_; }

		bool ImGuiWidget();

		const auto &GetTimer() const { return timer_; }

		bool isDelete_ = false;

		void CreateGoal();

	private:
		const float &lerpTime_;

		SoLib::DeltaTimer timer_;
		std::list<AABB> collisionBox_;

		std::unique_ptr<Entity> goal_;

		std::list<std::unique_ptr<Entity>> starItem_;

		std::list<Box> boxList_;

		std::list<Box>::iterator boxItr_;
	};

	struct StateLog {
		Entity *item_ = nullptr;
		int32_t remainRotCount_;
		std::list<std::pair<uint32_t, float>> angleList_;
	};

public:
	static auto *const GetInstance() {
		static LevelElementManager instance{};
		return &instance;
	}

	static void StaticInit();

	void LoadData(const uint32_t levelID);

	void SetData();

	bool AnyPlatformRotating() const;

	void Undo();

	void UndoUpdate(const float deltaTime);

	/// @brief 履歴を追加する
	void AddUndoLog(Entity *const starItem);

	void ImGuiWidget();

	void Init();

	void Update(float deltaTime);

	void Draw(const Camera3D &camera) const;

	void DebugDraw(const Camera3D &camera) const;

	void CalcCollision(const uint32_t key);
	void CalcCollision();

	/// @brief ブロックを追加
	/// @param transform ブロックのSRT
	void AddBlock(const uint32_t key, const AABB &box, GroundType groundType);

	void AddItem(const uint32_t key, const BaseTransform &srt);

	/// @brief プレイヤのセッター
	void SetPlayer(Entity *const player) { pPlayer_ = player; }
	/// @brief プレイヤのゲッター
	const auto *const GetPlayer() const { return pPlayer_; }

	const LineBase &GetStageLine() const { return stageLine_; }

	/// @brief 回転回数の変更
	/// @param count 回転回数の加算値
	void AddRotateCount(const int32_t count);

	Platform *const GetPlatform(int32_t index);

	using PlatformMap = std::map<uint32_t, Platform>;
	PlatformMap blockCollider_;

	const auto &GetGroundModel() const { return groundModels_; }

	VariantItem<float> vLerpTime_{ "LerpTime", 1.f };
	VariantItem<int32_t> vMaxRotateCount_{ "MaxRotateCount", 2 };

private:
	static nlohmann::json GetLevelParameters(const nlohmann::json &jsonData, int32_t levelIndex);

private:
	Vector3 startPos_;

	std::array<Model *, 2u> groundModels_ = {};
	Transform lineStart_;
	Transform lineEnd_;
	LineBase stageLine_;

	Entity *pPlayer_ = nullptr;

	SoLib::DeltaTimer undoTimer_;

	StateLog undoLog_;

	std::list<StateLog> stateLog_;

	void SetTransferData() const;

	int32_t remainRotateCount_;

	PlatformMap::iterator platformItr_;

	nlohmann::json monoLevelData_;

	static nlohmann::json levelData_;

	bool debugDrawer_ = false;
};