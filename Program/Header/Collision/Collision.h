/// @file Collision.h
/// @brief 当たり判定など
/// @author ONUKI seiya
#pragma once

#include "../../Engine/Utils/Math/Math.hpp"
#include "../../Engine/Utils/Math/Matrix4x4.h"
#include "../../Engine/Utils/Math/Transform.h"
#include "../../Engine/Utils/Math/Vector3.h"
#include <functional>
#include <string>
#include <variant>
#include <vector>

namespace SolEngine {
	class Render;
}

struct LineBase;
struct Plane;
struct Triangle;
struct Sphere;
struct AABB;
struct OBB;

struct Capsule;

namespace Collision {
	/// @brief 当たり判定
	const bool IsHit(const LineBase &line, const Plane &plane);
	inline const bool IsHit(const Plane &plane, const LineBase &line) { return IsHit(line, plane); }

	const bool IsHit(const Sphere &sphereA, const Sphere &sphereB);
	const bool IsHit(const Sphere &sphere, const Plane &plane);
	const bool IsHit(const LineBase &line, const Triangle &triangle);
	const bool IsHit(const AABB &a, const AABB &b);
	const bool IsHit(const AABB &aabb, const Sphere &sphere);
	const bool IsHit(const AABB &aabb, const LineBase &line);

	const bool IsHit(const OBB &obb, const Sphere &sphere);
	const bool IsHit(const OBB &obb, const LineBase &line);

	const bool IsHit(const OBB &obbA, const OBB &obbB);

	const bool IsHit(const Capsule &cupsele, const Plane &plane);

	/// @brief 任意の軸において､2つの箱は衝突しているか
	const bool IsHitAxis(const Vector3 &axis, const std::array<Vector3, 8u> vertexA, const std::array<Vector3, 8u> vertexB);

	/// @brief 衝突している進行度
	const float HitProgress(const LineBase &line, const Plane &plane);
	const float HitProgress(const LineBase &line, const AABB &aabb);

	/// @brief 衝突点
	const Vector3 HitPoint(const LineBase &line, const Plane &plane);

	// const bool

} // namespace Collision

struct IShape
{
};

struct OBB : public IShape
{
	Vector3 centor{};								// 中心点
	Quaternion orientations = Quaternion::Identity; // 回転
	Vector3 size{};									// 中心点からの各軸の半径

	/// @brief 回転の更新
	void SetRotate(const Vector3 &euler);

	/// @brief ワールド行列の生成
	const Matrix4x4 GetWorldMatrix() const;
	/// @brief ワールド行列の逆行列
	const Matrix4x4 GetInverseMatrix() const;

	void ImGuiDebug(const std::string &group, Vector3 &rotate);
};

struct Plane : public IShape
{
	Vector3 normal;
	float distance;

	/// @brief 平面の作成
	static Plane Create(const Vector3 &Normal, const Vector3 &Point);
	static Plane Create(const Vector3 Vertex[3])
	{
		return Create(((Vertex[1] - Vertex[0]).cross(Vertex[2] - Vertex[1])).Normalize(), Vertex[0]);
	}
	static Plane Create(const Triangle &trinagle);

	/// @brief 平面と点の距離
	float GetDistance(const Vector3 &point) const { return normal * point - distance; }
	/// @brief 平面と線の距離
	const bool IsCollision(const LineBase &other) const;
};

/// @brief 3角ポリゴン
struct Triangle : public IShape
{
	// 頂点リスト(時計回り)
	Vector3 vertices_[3] = {};

	Triangle() {}
	/// @param LocalVertices 頂点リスト(時計回り)
	Triangle(const Vector3 &Vertice0, const Vector3 &Vertice1, const Vector3 &Vertice2)
		: vertices_{Vertice0, Vertice1, Vertice2} {}

	Triangle(const Triangle &) = default;
	~Triangle() = default;

	[[nodiscard]] const Triangle operator*(const Matrix4x4 &matrix) const
	{
		Triangle out{};
		for (uint8_t i = 0; i < 3; i++) {
			out.vertices_[i] = (vertices_[i] * matrix);
		}
		return out;
	}

	/// @brief 法線を取得
	[[nodiscard]] Vector3 GetNormal() const
	{
		const Vector3 &VecA = vertices_[1] - vertices_[0]; // 0 から 1 に向けて
		const Vector3 &VecB = vertices_[2] - vertices_[1]; // 1 から 2 に向けて
		return (VecA.cross(VecB)).Normalize();
	}

	// void ImGuiDebug(const std::string &group);
};

struct Sphere : public IShape
{
	Vector3 centor;
	float radius;

	/// @brief 球と球の衝突
	bool IsCollision(const Sphere &other) const { return Collision::IsHit(*this, other); }
	bool IsCollision(const Plane &plane) const { return Collision::IsHit(*this, plane); }

	void ImGuiDebug(const std::string &group);
};

struct LineBase final : public IShape
{
	enum class LineType : uint32_t
	{
		Line,
		Ray,
		Segment
	};
	Vector3 origin; // 始点
	Vector3 diff;	// 終点へのベクトル
	LineType lineType = LineType::Segment;
	/// @brief 末端の設定
	void SetEnd(const Vector3 &end) { diff = end - origin; }

	/// @brief 終点の取得
	Vector3 GetEnd() const { return origin + diff; }
	/// @brief 進行度の取得
	Vector3 GetProgress(const float &t) const;
	/// @brief 線分と点の最も近いベクトルの取得
	Vector3 Project(const Vector3 &point) const;
	/// @brief 点に最も近い線上の座標を取得
	Vector3 ClosestPoint(const Vector3 &point) const;
	/// @brief 線のタイプでの進行度を変更する
	const float Clamp(const float &t) const;

	void ImGuiDebug(const std::string &group);

private:
	/// @brief 線と点の距離が最小の進行度
	[[nodiscard]] float ClosestProgress(const Vector3 &point) const;
	inline static std::array<const char *const, 3u> typeList = {"Line", "Ray", "Segment"};
};

struct AABB : public IShape
{
	Vector3 min;
	Vector3 max;

	/// @brief AABBを移動する
	const AABB AddPos(const Vector3 &vec) const;
	/// @brief AABBを拡大する
	AABB Extend(const Vector3 &vec) const;

	///	@brief 中心座標を取得
	Vector3 GetCentor() const;
	/// @brief 半径を取得
	Vector3 GetRadius() const;
	/// @brief 衝突点の法線を取得
	Vector3 GetNormal(const Vector3 &surface) const;
	Vector3 GetNormal(const Vector3 &surface, const Vector3 &direction) const;

	/// @brief Transform行列の生成
	/// @return transform行列
	Matrix4x4 TransMat() const;

	/// @brief 新規生成
	/// @param origin 原点
	/// @param radius 半径
	/// @return AABB構造体
	static AABB Create(const Vector3 &origin, const Vector3 &radius);

	void ImGuiDebug(const std::string &group);
	/// @brief 頂点情報を入れ替え
	const AABB &Swaping();

	/// @brief 全ての頂点情報を取得する
	/// @return 各種頂点 [ 下面4つ , 上面4つ]
	std::array<Vector3, 8u> GetVertex() const;
};

struct Ball
{
	Vector3 position;	  // ボールの位置
	Vector3 velocity;	  // ボールの速度
	Vector3 acceleration; // ボールの加速度
	float mass;			  // ボールの質量
	float radius;		  // ボールの半径
	uint32_t color;		  // ボールの色
	/// @brief ボールの更新
	void Update(const Plane &plane, const float deltaTime, const float elasticity);
};

struct Spring
{
	Vector3 anchor;			  // 固定された端
	float naturalLength;	  // 自然長
	float stiffness;		  // 剛性 ばね定数k
	float dampingCoefficient; // 減衰係数
	/// @brief ボールに与えられる加速度
	Vector3 GetAcceleration(const Ball &ball);
};

struct Pendulum
{
	Vector3 anchor;			   // 固定された端
	float length;			   // 紐の長さ
	float angle;			   // 現在の角度
	float angularVelocity;	   // 角速度ω
	float angularAcceleration; // 角加速度
	/// @brief 振り子を回転させる
	void MoveSwing(const Vector3 &kGravity, const float deltaTime);
};

struct ConicalPendulum
{
	Vector3 anchor;		   // 固定された端
	float length;		   // 紐の長さ
	float halfApexAngle;   // 円錐の頂点の半分
	float angle;		   // 現在の角度
	float angularVelocity; // 角速度ω
	/// @brief 振り子を回転させる
	void MoveAngle(const Vector3 &kGravity, const float deltaTime);
	/// @brief 現在の位置を取得
	Vector3 GetPos();
};

struct Capsule : public IShape
{
	LineBase segment{.lineType = LineBase::LineType::Segment};
	float radius;
	/// @brief 衝突した座標
	Vector3 GetHitPoint(const Plane &plane) const;
};

/// @brief AABB同士の結合
/// @return お互いの両端を取ったAABB
AABB operator+(const AABB &first, const AABB &second);

using VariantShapes = std::variant<std::byte, AABB, OBB, Sphere, Capsule /*, Cylinder*/>;

template <typename T>
concept IsShapeType = requires(const T shape) {
	{ VariantShapes{shape} } -> std::same_as<VariantShapes>;
};

class ShapesList
{

	// Variantの型リストを取得するテンプレート
	template <typename T>
	struct VariantTypes;

	template <typename... Ts>
	struct VariantTypes<std::variant<Ts...>>
	{
		using Types = std::tuple<std::list<Ts>...>;
	};

public:
	using ShapeTypes = VariantTypes<VariantShapes>::Types;

	ShapeTypes shapeTypes_;

	template <IsShapeType T>
	void push_back(const T &shape)
	{
		auto &list = std::get<std::list<T>>(shapeTypes_);
		list.push_back(shape);
	}
};
