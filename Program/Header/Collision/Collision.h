#pragma once

#include "../../Utils/Math/Math.hpp"
#include "../../Utils/Math/Matrix4x4.h"
#include "../../Utils/Math/Vector3.h"
#include "../../Utils/Math/Transform.h"
#include <functional>
#include <string>
#include <vector>
#include <variant>

class Render;
struct LineBase;
struct Plane;
struct Triangle;
struct Sphere;
struct AABB;
struct OBB;

struct Capsule;

namespace Collision {
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

	const bool IsHitAxis(const Vector3 &axis, const std::array<Vector3, 8u> vertexA, const  std::array<Vector3, 8u> vertexB);

	const float HitProgress(const LineBase &line, const Plane &plane);
	const float HitProgress(const LineBase &line, const AABB &aabb);

	const Vector3 HitPoint(const LineBase &line, const Plane &plane);

	// const bool

} // namespace Collision

struct IShape {

};

struct OBB : public IShape {
	Vector3 centor;									// 中心点
	Quaternion orientations = Quaternion::Identity; // ローカル座標
	Vector3 size;                                   // 中心点からの各軸の半径

	void SetRotate(const Vector3 &euler);

	const Matrix4x4 GetWorldMatrix() const;
	const Matrix4x4 GetInverseMatrix() const;

	void ImGuiDebug(const std::string &group, Vector3 &rotate);
};

struct Plane : public IShape {
	Vector3 normal;
	float distance;

	static Plane Create(const Vector3 &Normal, const Vector3 &Point);
	static Plane Create(const Vector3 Vertex[3]) {
		return Create(((Vertex[1] - Vertex[0]).cross(Vertex[2] - Vertex[1])).Nomalize(), Vertex[0]);
	}
	static Plane Create(const Triangle &trinagle);

	float GetDistance(const Vector3 &point) const { return normal * point - distance; }
	const bool IsCollision(const LineBase &other) const;
};

/// @brief 3角ポリゴン
struct Triangle : public IShape {
	// 頂点リスト(時計回り)
	Vector3 vertices_[3] = {};

	Triangle() {}
	/// @param LocalVertices 頂点リスト(時計回り)
	Triangle(const std::array<Vector3, 3u> Vertices);
	Triangle(const Vector3 &Vertice0, const Vector3 &Vertice1, const Vector3 &Vertice2)
		: vertices_{ Vertice0, Vertice1, Vertice2 } {}

	Triangle(const Triangle &) = default;
	~Triangle();

	[[nodiscard]] const Triangle operator*(const Matrix4x4 &matrix) const {
		Triangle out{};
		for (uint8_t i = 0; i < 3; i++) {
			out.vertices_[i] = (vertices_[i] * matrix);
		}
		return out;
	}

	[[nodiscard]] Vector3 GetNormal() const {
		const Vector3 &VecA = vertices_[1] - vertices_[0]; // 0 から 1 に向けて
		const Vector3 &VecB = vertices_[2] - vertices_[1]; // 1 から 2 に向けて
		return (VecA.cross(VecB)).Nomalize();
	}

	void ImGuiDebug(const std::string &group);
};

struct Sphere : public IShape {
	Vector3 centor;
	float radius;

	bool IsCollision(const Sphere &other) const { return Collision::IsHit(*this, other); }
	bool IsCollision(const Plane &plane) const { return Collision::IsHit(*this, plane); }

	void ImGuiDebug(const std::string &group);
};

struct LineBase final : public IShape {
	enum class LineType : uint32_t { Line, Ray, Segment };
	Vector3 origin; // 始点
	Vector3 diff;   // 終点へのベクトル
	LineType lineType = LineType::Segment;

	void SetEnd(const Vector3 &end) { diff = end - origin; }

	Vector3 GetEnd() const { return origin + diff; }
	Vector3 GetProgress(const float &t) const;
	Vector3 Project(const Vector3 &point) const;
	Vector3 ClosestPoint(const Vector3 &point) const;
	const float Clamp(const float &t) const;

	void ImGuiDebug(const std::string &group);

private:
	[[nodiscard]] float ClosestProgress(const Vector3 &point) const;
	inline static std::array<const char *const, 3u> typeList = { "Line", "Ray", "Segment" };
};

struct AABB : public IShape {
	Vector3 min;
	Vector3 max;

	const AABB AddPos(const Vector3 &vec) const;
	AABB Extend(const Vector3 &vec) const;

	Vector3 GetCentor() const;
	Vector3 GetRadius() const;

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
	const AABB &Swaping();

	/// @brief 全ての頂点情報を取得する
	/// @return 各種頂点 [ 下面4つ , 上面4つ]
	std::array<Vector3, 8u> GetVertex() const;
};

struct Ball {
	Vector3 position;     // ボールの位置
	Vector3 velocity;     // ボールの速度
	Vector3 acceleration; // ボールの加速度
	float mass;           // ボールの質量
	float radius;         // ボールの半径
	uint32_t color;       // ボールの色

	void Update(const Plane &plane, const float deltaTime, const float elasticity);
};

struct Spring {
	Vector3 anchor;           // 固定された端
	float naturalLength;      // 自然長
	float stiffness;          // 剛性 ばね定数k
	float dampingCoefficient; // 減衰係数

	Vector3 GetAcceleration(const Ball &ball);
};

struct Pendulum {
	Vector3 anchor;            // 固定された端
	float length;              // 紐の長さ
	float angle;               // 現在の角度
	float angularVelocity;     // 角速度ω
	float angularAcceleration; // 角加速度
	void MoveSwing(const Vector3 &kGravity, const float deltaTime);
};

struct ConicalPendulum {
	Vector3 anchor;        // 固定された端
	float length;          // 紐の長さ
	float halfApexAngle;   // 円錐の頂点の半分
	float angle;           // 現在の角度
	float angularVelocity; // 角速度ω
	void MoveAngle(const Vector3 &kGravity, const float deltaTime);
	Vector3 GetPos();
};

struct Capsule : public IShape {
	LineBase segment{ .lineType = LineBase::LineType::Segment };
	float radius;

	Vector3 GetHitPoint(const Plane &plane) const;
};

/// @brief AABB同士の結合
/// @return お互いの両端を取ったAABB
AABB operator+(const AABB &first, const AABB &second);

using VariantShapes = std::variant<AABB, OBB, Sphere, Capsule/*, Cylinder*/>;

class ShapesList {

	// Variantの型リストを取得するテンプレート
	template <typename T>
	struct VariantTypes;

	template <typename... Ts>
	struct VariantTypes<std::variant<Ts...>> {
		using Types = std::tuple<std::list<Ts>...>;
	};

public:

	using ShapeTypes = VariantTypes<VariantShapes>::Types;

	ShapeTypes shapeTypes_;
	/*template <typename T>
	void push_back(const T &shape) {
		auto &list = std::get<std::list<T>>(shapeTypes_);
		list.push_back(shape);
	}*/
};
