#pragma once
#include "../../Engine/DirectBase/2D/Sprite.h"
#include "../../Utils/Math/Angle.h"
#include "Enemy.h"
#include <memory>

class LockOn {
public:

	enum class LockOnAngle {
		kAll,
		kRight,
		kLeft,
	};
	LockOn() = default;
	~LockOn() = default;
	/// @brief 初期化
	void Init();
	/// @brief ターゲットの破棄
	void Reset();
	/// @brief 更新
	void Update(const std::list<std::unique_ptr<Enemy>> &enemyList, const Camera3D &camera);
	/// @brief 描画
	void Draw2D() const;

	const auto *GetTarget() const { return target_; }

	/// @brief ロックオン中か
	bool IsLockOn() const { return target_ != nullptr; }

	/// @brief ターゲットのワールド座標の取得
	/// @return ターゲットのワールド座標
	Vector3 GetTargetPosition() const { return Vector3::up * target_->GetWorldTransform()->matWorld_; }

private:
	std::list<std::pair<float, const Enemy *>> FindTarget(const std::list<std::unique_ptr<Enemy>> &enemyList, const Camera3D &camera, LockOnAngle lockType = LockOnAngle::kAll) const;

	bool IsCanLockOn(const Enemy *const target, const Camera3D &camera) const;

	/// @brief 最小距離
	float minDistance_ = 10.f;
	/// @brief 最大距離
	float maxDistance_ = 50.f;

	float angleRange_ = 20._deg;

	float lockAngle_ = std::cos(30._deg);

	float preInputStick_;

	std::unique_ptr<Sprite> sprite_ = nullptr;
	const Enemy *target_ = nullptr;
};
