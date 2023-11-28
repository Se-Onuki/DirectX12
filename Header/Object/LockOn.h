#pragma once
#include "../../Engine/DirectBase/2D/Sprite.h"
#include "../../Utils/Math/Angle.h"
#include "Enemy.h"
#include <memory>

class LockOn {
public:
    LockOn() = default;
    ~LockOn() = default;
    /// @brief 初期化
    void Init();
    /// @brief 更新
    void Update(const std::list<std::unique_ptr<Enemy>> &enemyList, const Camera3D &camera);
    /// @brief 描画
    void Draw2D() const;

private:
    const Enemy *const FindTarget(const std::list<std::unique_ptr<Enemy>> &enemyList, const Camera3D &camera)const;

    /// @brief 最小距離
    float minDistance_ = 10.f;
    /// @brief 最大距離
    float maxDistance_ = 30.f;

    float angleRange_ = 20._deg;

    float lockAngle_ = std::cos(45._deg);

    std::unique_ptr<Sprite> sprite_ = nullptr;
    const Enemy *target_ = nullptr;
};
