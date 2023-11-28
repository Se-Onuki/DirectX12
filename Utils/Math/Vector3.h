#pragma once
// #include "Matrix3x3.hpp"
#include "Vector2.h"

#include "SimdCalc.h"
#include <array>
#include <cmath>
#include <immintrin.h>

struct Matrix4x4;

struct Vector3 {
    // Vector3(float x = 0.f, float y = 0.f, float z = 0.f) : x(x), y(y), z(z) {}

    float x;
    float y;
    float z;

    /// <summary>
    /// ベクトルをデバック表示する
    /// </summary>
    /// <param name="x">x座標</param>
    /// <param name="y">y座標</param>
    void Printf(int x, int y) const;

    /// <summary>
    /// ベクトル長関数
    /// </summary>
    /// <returns>ベクトルの長さ</returns>
    float Length() const { return sqrtf((*this) * (*this)); }

    /// @brief 2乗ベクトル長関数
    /// @return ベクトル長
    float LengthSQ() const { return (*this) * (*this); }

    /// <summary>
    /// 正規化
    /// </summary>
    /// <returns>ベクトル長が1のベクトル</returns>
    Vector3 Nomalize() const
    {

        float Length = this->Length();
        if (Length != 0) {
            return *this / Length;
        }
        else {
            return zero;
        }
    }

    Vector3 operator+(const Vector3 &Second) const
    {
        return Vector3{this->x + Second.x, this->y + Second.y, this->z + Second.z};
    }
    Vector3 operator-(const Vector3 &Second) const
    {
        return Vector3{this->x - Second.x, this->y - Second.y, this->z - Second.z};
    }

    Vector3 &operator+=(const Vector3 &Second)
    {
        this->x += Second.x;
        this->y += Second.y;
        this->z += Second.z;
        return *this;
    }
    Vector3 &operator-=(const Vector3 &Second)
    {
        this->x -= Second.x;
        this->y -= Second.y;
        this->z -= Second.z;
        return *this;
    }

    Vector3 operator*(const float &Second) const
    {
        return Vector3{this->x * Second, this->y * Second, this->z * Second};
    }
    Vector3 operator/(const float &Second) const
    {
        return Vector3{this->x / Second, this->y / Second, this->z / Second};
    }

    Vector3 &operator*=(const float &Second)
    {
        this->x *= Second;
        this->y *= Second;
        this->z *= Second;
        return *this;
    }
    Vector3 &operator/=(const float &Second)
    {
        this->x /= Second;
        this->y /= Second;
        this->z /= Second;
        return *this;
    }

    // Vector3 operator*(const Matrix3x3& Second) const {}

    // Vector3 operator*=(const Matrix3x3& Second) {}

    Vector3 operator*(const Matrix4x4 &Second) const;

    Vector3 &operator*=(const Matrix4x4 &Second);

    inline const Vector3 &operator+() const { return *this; }
    // 逆ベクトル
    inline Vector3 operator-() const { return *this * -1; }

    // 内積
    inline float operator*(const Vector3 &v) const { return x * v.x + y * v.y + z * v.z; }
    // 外積(クロス積)
    [[nodiscard]] inline Vector3 cross(const Vector3 &v) const { return Vector3{y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x}; }

    /// @brief 垂直ベクトル
    /// @return 90度回転したベクトル
    inline Vector3 Perpendicular() const
    {
        if (x != 0.f || y != 0.f)
            return Vector3{-y, x, 0.f};
        return Vector3{0.f, -z, y};
    }

    static const Vector3 zero;

    static const Vector3 up;

    static const Vector3 front;

    static const Vector3 right;

    static const Vector3 one;

    inline Vector3 Reflect(Vector3 normal) const { return (*this) - normal * 2 * ((*this) * normal); }

    inline bool operator==(const Vector3 &vec) const { return (this->x == vec.x) && (this->y == vec.y) && (this->z == vec.z); }

    Vector3 Direction2Euler() const
    {
        Vector3 out{};
        if (x == 0 && z == 0) {
            out.y = 0;
        }
        else {
            out.y = std::atan2(x, z);
        }

        if (y == 0) {
            out.x = 0;
        }
        else {
            out.x = std::atan2(-y, std::sqrt(std::powf(x, 2) + std::powf(z, 2)));
        }

        return out;
    }

    Vector3 Scaling(const Vector3 &other) const;

    const Vector2 &ToVec2() const { return *reinterpret_cast<const Vector2 *>(this); }

    /// @brief 暗黙的な配列への変換
    inline operator std::array<float, 3u> &() noexcept
    {
        return *reinterpret_cast<std::array<float, 3u> *>(&x);
    }

    /// @brief 暗黙的な配列への変換
    inline operator const std::array<float, 3u> &() const noexcept
    {
        return *reinterpret_cast<const std::array<float, 3u> *>(&x);
    }

    static uint32_t size() { return 3u; }

    float *const begin() { return &x; }
    const float *const begin() const { return &x; }
    const float *const cbegin() const { return &x; }

    float *const end() { return begin() + size(); }
    const float *const end() const { return end(); }
    const float *const cend() const { return end(); }

    float *const data() { return begin(); }
    const float *const data() const { return begin(); }
    const float *const cdata() const { return begin(); }

    inline explicit operator __m128() const { return _mm_setr_ps(x, y, z, 1.f); }
    inline Vector3 &operator=(const __m128 &vec)
    {
        std::memcpy(this, &vec, sizeof(*this));
        return *this;
    }

    inline explicit operator SoLib::Math::SIMD128() const { return SoLib::Math::SIMD128{static_cast<__m128>(*this)}; }

private:
};

inline Vector3 operator*(const float &left, const Vector3 &right)
{
    return right * left;
}

// namespace std {
//	template<>
//	struct hash<Vector3> {
//		size_t operator()(const Vector3 &v) const {
//			std::string s = std::to_string(v.x) + std::to_string(v.y) + std::to_string(v.z);
//			return std::hash<std::string>()(s);
//		}
//	};
// }