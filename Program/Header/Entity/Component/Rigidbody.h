/// @file Rigidbody.h
/// @brief 物理挙動のコンポーネント
/// @author ONUKI seiya
#pragma once
#include "../Entity.h"

#include "../../Engine/Utils/Math/Vector3.h"

class Rigidbody : public IComponent {
public:
	using IComponent::IComponent;
	~Rigidbody() = default;

	void Init();
	void Update() override;

	/// @brief 瞬間的な加速を行う
	/// @param vec 加速度
	void ApplyInstantForce(const Vector3 &vec) { acceleration_ += vec; }

	/// @brief 継続的な加速
	/// @param vec 加速度
	/// @param deltaTime 時間差分
	void ApplyContinuousForce(const Vector3 &vec) { acceleration_ += vec * GetDeltaTime(); }

	/// @brief 加速度をセット
	void SetAcceleration(const Vector3 &vec) { acceleration_ = vec; }
	/// @brief 加速度を取得
	const Vector3 &GetAcceleration() const { return acceleration_; }

	/// @brief 速度をセット
	void SetVelocity(const Vector3 &vec) { velocity_ = vec; }
	/// @brief 速度を取得
	const Vector3 &GetVelocity() const { return velocity_; }
	/// @brief 着地しているかどうか
	bool GetIsGround() const { return isGround_; }
	/// @brief 着地している状態にする
	void SetIsGround(const bool isGround) { isGround_ = isGround; }

	/// @brief 最大速度を取得
	const Vector3 &GetMaxSpeed() const { return maxSpeed_; }
	/// @brief 最大速度をセット
	void SetMaxSpeed(const Vector3 &hoge) { maxSpeed_ = hoge; }

	bool hasCollider_ = false;
	float radius_ = 1.f;

	/// @brief 前フレームの座標
	const Vector3 &GetBeforePos() const { return beforePos_; }

private:
	Vector3 velocity_{};
	Vector3 acceleration_{};

	Vector3 beforePos_;

	Vector3 maxSpeed_{ -1.f,-1.f,-1.f };	// 負数である場合は処理しない

	bool isGround_ = false;
};
