/// @file Vector2.h
/// @brief Vector2
/// @author ONUKI seiya
#pragma once
#include <cmath>

struct Matrix2x2;
struct Matrix3x3;

struct Vector2 {
	//Vector2(float x = 0.f, float y = 0.f) : x(x), y(y) {}


	float x;
	float y;

	/// <summary>
	/// ベクトルをデバック表示する
	/// </summary>
	/// <param name="x">x座標</param>
	/// <param name="y">y座標</param>
	//void Printf(int x, int y) const;

	/// <summary>
	/// ベクトルを回転させる
	/// </summary>
	/// <param name="theta">回転角</param>
	/// <returns>回転したベクトル</returns>
	Vector2 Rotate(const float &theta) const;

	/// <summary>
	/// ゼロベクトル
	/// </summary>
	void Reset();

	/// <summary>
	/// ベクトル長関数
	/// </summary>
	/// <returns>ベクトルの長さ</returns>
	float Length() const;

	/// <summary>
	/// 正規化
	/// </summary>
	/// <returns>ベクトル長が1のベクトル</returns>
	Vector2 Nomalize() const;

	Vector2 operator+(const Vector2 Second) const;
	Vector2 operator-(const Vector2 Second) const;

	void operator+=(const Vector2 Second);
	void operator-=(const Vector2 Second);

	Vector2 operator*(const float &Second) const;
	Vector2 operator/(const float &Second) const;

	void operator*=(const float &Second);
	void operator/=(const float &Second);

	Vector2 operator*(const Matrix2x2 &Second) const;

	void operator*=(const Matrix2x2 &Second);

	Vector2 operator*(const Matrix3x3 &Second) const;

	void operator*=(const Matrix3x3 &Second);

	inline float GetTheta() const {
		if (*this == Vector2{ 0, 0 }) {
			return 0;
		}
		return atan2(y, x);
	}

	// 逆ベクトル
	inline Vector2 operator-() const {
		return *this * -1;
	}

	// 内積
	inline float operator*(const Vector2 v) const {
		return x * v.x + y * v.y;
	}
	// 外積
	inline float operator^(const Vector2 v) const {
		return x * v.y - y * v.x;
	}

	static const Vector2 zero;
	static const Vector2 right;
	static const Vector2 up;
	static const Vector2 one;

	inline Vector2 Reflect(Vector2 normal) const {
		return (*this) - normal * 2 * ((*this) * normal);

		// return {this->x- 2}
	}

	inline bool operator==(const Vector2 vec) const {
		return (this->x == vec.x) && (this->y == vec.y);
	}

	static uint32_t size() { return 2u; }

	float *const begin() { return &x; }
	const float *const begin() const { return &x; }
	const float *const cbegin() const { return &x; }

	float *const end() { return begin() + size(); }
	const float *const end() const { return begin() + size(); }
	const float *const cend() const { return begin() + size(); }

	float *const data() { return begin(); }
	const float *const data() const { return begin(); }
	const float *const cdata() const { return begin(); }

private:
};

inline Vector2 operator*(const float &A, const Vector2 B) {
	return B * A;
}

/// <summary>
/// int型の二次元ベクトル
/// </summary>
/// <param name="x">横</param>
/// <param name="y">縦</param>
class IntVector2 {
public:
	inline IntVector2(Vector2 vec = { 0, 0 }) : x(int(vec.x)), y(int(vec.y)) {}
	int x;
	int y;
};

constexpr Vector2 ScreenSize{ 1280, 720 };

/// ゼロベクトル
constexpr Vector2 ZeroVector2 = { 0.f, 0.f };

/// UIなどの基準値となる座標
constexpr Vector2 MiddleLeftOver = { (float)(-ScreenSize.x / 2), (float)(ScreenSize.y / 2) };
constexpr Vector2 TopLeftOver = { (float)(-ScreenSize.x / 2), (float)(ScreenSize.y) };
constexpr Vector2 DownLeftOver = { (float)(-ScreenSize.x / 2), 0 };

constexpr Vector2 MiddleLeft = { 0, (float)(ScreenSize.y / 2) };
constexpr Vector2 TopLeft = { 0, (float)(ScreenSize.y) };
constexpr Vector2 DownLeft = { 0, 0 };

constexpr Vector2 MiddleCentor{ (float)(ScreenSize.x / 2), (float)(ScreenSize.y / 2) };
constexpr Vector2 TopCentor = { (float)(ScreenSize.x / 2), (float)(ScreenSize.y) };
constexpr Vector2 DownCentor = { (float)(ScreenSize.x / 2), 0 };

constexpr Vector2 MiddleRight = { (float)(ScreenSize.x), (float)(ScreenSize.y / 2) };
constexpr Vector2 TopRight = { (float)(ScreenSize.x), (float)(ScreenSize.y) };
constexpr Vector2 DownRight = { (float)(ScreenSize.x), 0 };

constexpr Vector2 MiddleRightOver = { (float)(ScreenSize.x * 1.5), (float)(ScreenSize.y / 2) };
constexpr Vector2 TopRightOver = { (float)(ScreenSize.x * 1.5), (float)(ScreenSize.y) };
constexpr Vector2 DownRightOver = { (float)(ScreenSize.x * 1.5), 0 };

constexpr Vector2 TopOverCentor = { (float)(ScreenSize.x * 0.5), (float)(-ScreenSize.y / 2) };
constexpr Vector2 DownOverCentor = { (float)(ScreenSize.x * 0.5), (float)(ScreenSize.y * 1.5) };
