/// @file Collision.cpp
/// @brief 当たり判定など
/// @author ONUKI seiya
#include "Collision.h"

#include "../../Engine/Utils/SoLib/SoLib_Lerp.h"
#include "../../Engine/DirectBase/Render/Render.h"
#include <algorithm>

#include <imgui.h>

Vector3 LineBase::GetProgress(const float &t) const { return origin + diff * t; }

Vector3 LineBase::Project(const Vector3 &point) const {
	return (point - origin) - ClosestProgress(point) * diff;
}

Vector3 LineBase::ClosestPoint(const Vector3 &point) const {
	return ClosestProgress(point) * diff + origin;
}

const float LineBase::Clamp(const float &t) const {
	switch (lineType) {
	case LineBase::LineType::Line:
		return t;
		break;
	case LineBase::LineType::Ray:
		return (t > 0.f) ? t : 0.f;
		break;
	case LineBase::LineType::Segment:
		return std::clamp(t, 0.f, 1.f);
		break;
	default:
		return 0.f;
		break;
	}
}


void LineBase::ImGuiDebug(const std::string &group) {
	if (ImGui::TreeNode(group.c_str())) {

		ImGui::DragFloat3("Origin", &origin.x, 0.1f);
		ImGui::DragFloat3("Diff", &diff.x, 0.1f);
		if (ImGui::BeginCombo("LineType", typeList[(uint8_t)lineType])) {

			for (uint8_t i = 0u; i < 3u; i++) {
				if (ImGui::Selectable(typeList[i])) {
					lineType = (LineType)i;
					break;
				}
			}
			ImGui::EndCombo();
		}
		ImGui::TreePop();
	}
}

float LineBase::ClosestProgress(const Vector3 &point) const {
	return Clamp(((point - origin) * diff) / std::powf(diff.Length(), 2));
}

Plane Plane::Create(const Vector3 &Normal, const Vector3 &Point) {
	Plane out;

	LineBase line{ .origin = Vector3::zero, .diff = Normal, .lineType = LineBase::LineType::Line };
	Vector3 centor = line.ClosestPoint(Point);
	if (centor != Vector3::zero) {
		out.normal = centor.Nomalize();
		out.distance = centor.Length();
	}
	else {
		out.normal = Normal;
		out.distance = Point * Normal;
	}

	return out;
}
Plane Plane::Create(const Triangle &trinagle) { return Create(trinagle.vertices_); }

const bool Plane::IsCollision(const LineBase &other) const {
	return Collision::IsHit(other, *this);
}

const bool Collision::IsHit(const LineBase &line, const Plane &plane) {
	const float dot = plane.normal * line.diff;
	if (dot == 0.f)
		return false;
	const float t = (plane.distance - (line.origin * plane.normal)) / dot;
	const float clampT = line.Clamp(t);
	return clampT == t;
}

const bool Collision::IsHit(const Sphere &sphereA, const Sphere &sphereB) {
	return (sphereA.centor - sphereB.centor).Length() <= sphereA.radius + sphereB.radius;
}

const bool Collision::IsHit(const Sphere &sphere, const Plane &plane) {
	return std::abs(plane.GetDistance(sphere.centor)) <= sphere.radius;
}

const bool Collision::IsHit(const LineBase &line, const Triangle &triangle) {
	Plane plane = Plane::Create(triangle);
	if (!IsHit(line, plane))
		return false;
	const Vector3 pos = HitPoint(line, plane);
	const Vector3 closs[3] = {
		{(triangle.vertices_[1] - triangle.vertices_[0]).cross(pos - triangle.vertices_[1])},
		{(triangle.vertices_[2] - triangle.vertices_[1]).cross(pos - triangle.vertices_[2])},
		{(triangle.vertices_[0] - triangle.vertices_[2]).cross(pos - triangle.vertices_[0])},
	};
	const Vector3 &normal = triangle.GetNormal();
	return ((normal * closs[0]) >= 0.f && (normal * closs[1]) >= 0.f && (normal * closs[2]) >= 0.f);
}

const bool Collision::IsHit(const AABB &a, const AABB &b) {
	return (
		(a.min.x <= b.max.x && a.max.x >= b.min.x) && (a.min.y <= b.max.y && a.max.y >= b.min.y) &&
		(a.min.z <= b.max.z && a.max.z >= b.min.z));
}

const bool Collision::IsHit(const AABB &aabb, const Sphere &sphere) {
	Vector3 clampPos{
		std::clamp(sphere.centor.x, aabb.min.x, aabb.max.x),
		std::clamp(sphere.centor.y, aabb.min.y, aabb.max.y),
		std::clamp(sphere.centor.z, aabb.min.z, aabb.max.z),
	};
	return ((clampPos - sphere.centor).Length() <= sphere.radius);
}

const bool Collision::IsHit(const AABB &aabb, const LineBase &line) {

	const Vector3 tMinVec{
		{(aabb.min.x - line.origin.x) / line.diff.x},
		{(aabb.min.y - line.origin.y) / line.diff.y},
		{(aabb.min.z - line.origin.z) / line.diff.z} };
	const Vector3 tMaxVec{
		{(aabb.max.x - line.origin.x) / line.diff.x},
		{(aabb.max.y - line.origin.y) / line.diff.y},
		{(aabb.max.z - line.origin.z) / line.diff.z} };

	const Vector3 tNear{
		min(tMinVec.x, tMaxVec.x), min(tMinVec.y, tMaxVec.y), min(tMinVec.z, tMaxVec.z) };
	const Vector3 tFar{
		max(tMinVec.x, tMaxVec.x), max(tMinVec.y, tMaxVec.y), max(tMinVec.z, tMaxVec.z) };

	const float tMin{ max(max(tNear.x, tNear.y), tNear.z) };
	const float tMax{ min(min(tFar.x, tFar.y), tFar.z) };
	if (tMin > 1.f && tMin != line.Clamp(tMin))
		return false;
	if (tMax < 0.f && tMax != line.Clamp(tMax))
		return false;
	return tMin <= tMax;
}

const bool Collision::IsHit(const OBB &obb, const Sphere &sphere) {
	Sphere localSphere = {
		.centor = sphere.centor * obb.GetInverseMatrix(), .radius = sphere.radius };
	AABB localOBB{ .min = -obb.size, .max = obb.size };
	return IsHit(localOBB, localSphere);
}

const bool Collision::IsHit(const OBB &obb, const LineBase &line) {
	AABB localOBB{ .min = -obb.size, .max = obb.size };

	const Vector3 &localOrigin = line.origin * obb.GetInverseMatrix();
	const Vector3 &localEnd = line.GetEnd() * obb.GetInverseMatrix();

	LineBase localLine{
		.origin = localOrigin, .diff = localEnd - localOrigin, .lineType = line.lineType };

	return IsHit(localOBB, localLine);
}

const bool Collision::IsHit(const OBB &obbA, const OBB &obbB) {
	std::array<Vector3, 8u> objA{
		{
			// lower
			{-obbA.size.x, -obbA.size.y, -obbA.size.z},
			{-obbA.size.x, -obbA.size.y, +obbA.size.z},
			{+obbA.size.x, -obbA.size.y, +obbA.size.z},
			{+obbA.size.x, -obbA.size.y, -obbA.size.z},
			// higher
			{-obbA.size.x, +obbA.size.y, -obbA.size.z},
			{-obbA.size.x, +obbA.size.y, +obbA.size.z},
			{+obbA.size.x, +obbA.size.y, +obbA.size.z},
			{+obbA.size.x, +obbA.size.y, -obbA.size.z},
		}
	};
	const Matrix4x4 &worldA = obbA.GetWorldMatrix();
	for (uint8_t i = 0; i < 8u; i++) {
		objA[i] *= worldA;
	}

	std::array<Vector3, 8u> objB{
		{
			// lower
			{-obbB.size.x, -obbB.size.y, -obbB.size.z},
			{-obbB.size.x, -obbB.size.y, +obbB.size.z},
			{+obbB.size.x, -obbB.size.y, +obbB.size.z},
			{+obbB.size.x, -obbB.size.y, -obbB.size.z},
			// higher
			{-obbB.size.x, +obbB.size.y, -obbB.size.z},
			{-obbB.size.x, +obbB.size.y, +obbB.size.z},
			{+obbB.size.x, +obbB.size.y, +obbB.size.z},
			{+obbB.size.x, +obbB.size.y, -obbB.size.z},
		}
	};
	const Matrix4x4 &worldB = obbB.GetWorldMatrix();
	for (uint8_t i = 0; i < 8u; i++) {
		objB[i] *= worldB;
	}

	const std::array<Vector3, 3u> &aOrientation = { obbA.orientations.GetRight(), obbA.orientations.GetUp(), obbA.orientations.GetFront() };
	const std::array<Vector3, 3u> &bOrientation = { obbB.orientations.GetRight(), obbB.orientations.GetUp(), obbB.orientations.GetFront() };

	return IsHitAxis(aOrientation[0], objA, objB) &&
		IsHitAxis(aOrientation[1], objA, objB) &&
		IsHitAxis(aOrientation[2], objA, objB) &&

		IsHitAxis(bOrientation[0], objA, objB) &&
		IsHitAxis(bOrientation[1], objA, objB) &&
		IsHitAxis(bOrientation[2], objA, objB) &&

		IsHitAxis(aOrientation[0].cross(bOrientation[0]).Nomalize(), objA, objB) &&
		IsHitAxis(aOrientation[0].cross(bOrientation[1]).Nomalize(), objA, objB) &&
		IsHitAxis(aOrientation[0].cross(bOrientation[2]).Nomalize(), objA, objB) &&

		IsHitAxis(aOrientation[1].cross(bOrientation[0]).Nomalize(), objA, objB) &&
		IsHitAxis(aOrientation[1].cross(bOrientation[1]).Nomalize(), objA, objB) &&
		IsHitAxis(aOrientation[1].cross(bOrientation[2]).Nomalize(), objA, objB) &&

		IsHitAxis(aOrientation[2].cross(bOrientation[0]).Nomalize(), objA, objB) &&
		IsHitAxis(aOrientation[2].cross(bOrientation[1]).Nomalize(), objA, objB) &&
		IsHitAxis(aOrientation[2].cross(bOrientation[2]).Nomalize(), objA, objB);
}

const bool Collision::IsHit(const Capsule &cupsele, const Plane &plane) {

	// 線分自体が当たっていたら
	if (IsHit(cupsele.segment, plane)) {
		return true;
	}
	return (
		std::abs(plane.normal * cupsele.segment.origin - plane.distance) <= cupsele.radius ||
		std::abs(plane.normal * cupsele.segment.GetEnd() - plane.distance) <= cupsele.radius);

	//return false;
}

const bool
Collision::IsHitAxis(const Vector3 &axis, const  std::array<Vector3, 8u> vertexA, const  std::array<Vector3, 8u> vertexB) {

	float minA = 0.f, maxA = 0.f;
	float minB = 0.f, maxB = 0.f;
	for (uint8_t i = 0u; i < 8u; i++) {
		const float A = axis * vertexA[i];
		if (i == 0u) {
			minA = A;
			maxA = A;
		}
		else {
			minA = min(minA, A);
			maxA = max(maxA, A);
		}

		const float B = axis * vertexB[i];
		if (i == 0u) {
			minB = B;
			maxB = B;
		}
		else {
			minB = min(minB, B);
			maxB = max(maxB, B);
		}
	}
	const float diffA = maxA - minA;
	const float diffB = maxB - minB;

	const float diffAll = max(maxA, maxB) - min(minA, minB);
	return diffAll <= diffA + diffB;
}

const float Collision::HitProgress(const LineBase &line, const Plane &plane) {
	const float dot = plane.normal * line.diff;
	if (dot == 0.f) {
		return 1.f; // 平行
	}
	return (plane.distance - (line.origin * plane.normal)) / dot;
}

const float Collision::HitProgress(const LineBase &line, const AABB &aabb) {

	const Vector3 tMinVec{
		{(aabb.min.x - line.origin.x) / line.diff.x},
		{(aabb.min.y - line.origin.y) / line.diff.y},
		{(aabb.min.z - line.origin.z) / line.diff.z} };
	const Vector3 tMaxVec{
		{(aabb.max.x - line.origin.x) / line.diff.x},
		{(aabb.max.y - line.origin.y) / line.diff.y},
		{(aabb.max.z - line.origin.z) / line.diff.z} };

	const Vector3 tNear{
		min(tMinVec.x, tMaxVec.x), min(tMinVec.y, tMaxVec.y), min(tMinVec.z, tMaxVec.z) };
	const Vector3 tFar{
		max(tMinVec.x, tMaxVec.x), max(tMinVec.y, tMaxVec.y), max(tMinVec.z, tMaxVec.z) };

	const float tMin{ max(max(tNear.x, tNear.y), tNear.z) };
	const float tMax{ min(min(tFar.x, tFar.y), tFar.z) };
	if (tMin > 1.f && tMin != line.Clamp(tMin)) {
		return 1.f;
	}
	if (tMax < 0.f && tMax != line.Clamp(tMax)) {
		return 1.f;
	}
	if (tMin <= tMax) {
		return line.Clamp(tMin);
	}
	return 1.f;
}
const Vector3 Collision::HitPoint(const LineBase &line, const Plane &plane) {
	const float dot = plane.normal * line.diff;
	if (dot == 0.f) {
		return Vector3::zero; // 平行
	}
	const float t = (plane.distance - (line.origin * plane.normal)) / dot;
	return line.GetProgress(t);
}

const AABB AABB::AddPos(const Vector3 &vec) const {
	AABB result = *this;
	result.min += vec;
	result.max += vec;

	return result;
}

AABB AABB::Extend(const Vector3 &vec) const {

	AABB result = *this;
	result.Swaping();

	for (uint32_t i = 0u; i < 3u; ++i) {
		// もし正の数なら
		if ((&vec.x)[i] > 0.f) {
			// maxを加算
			(&result.max.x)[i] += (&vec.x)[i];
		}
		// もし負数なら
		else {
			// minに加算
			(&result.min.x)[i] += (&vec.x)[i];
		}
	}

	return result;
}

Vector3 AABB::GetCentor() const {
	return SoLib::Lerp(min, max, 0.5f);
}

Vector3 AABB::GetRadius() const {
	return (max - min) / 2.f;
}

Vector3 AABB::GetNormal(const Vector3 &surface) const {
	Vector3 result{};
	// 各法線の軸
	static const std::array<Vector3, 3u> normalArray{
		Vector3::up,
		Vector3::right,
		Vector3::front,
	};
	// スケーリングした表面座標
	const Vector3 scalingSurface = (surface - this->GetCentor()).Scaling(this->GetRadius());

	for (auto &normal : normalArray) {
		// 法線情報との内積
		const Vector3 dot = normal * (normal * scalingSurface);
		// dotの絶対値が大きい要素を返す
		if (result.LengthSQ() < dot.LengthSQ()) {
			result = dot;
		}
	}

	return result.Nomalize();
}

Vector3 AABB::GetNormal(const Vector3 &surface, const Vector3 &direction) const {
	direction;
	Vector3 result{};
	// 各法線の軸
	static const std::array<Vector3, 3u> normalArray{
		Vector3::up,
		Vector3::right,
		Vector3::front,
	};
	// スケーリングした表面座標
	const Vector3 scalingSurface = (surface - this->GetCentor()).Scaling(this->GetRadius());

	for (auto &normal : normalArray) {
		// 法線情報との内積
		const Vector3 dot = normal * (normal * scalingSurface);
		// dotの絶対値が大きい要素を返す
		if (result.LengthSQ() < dot.LengthSQ()) {
			result = dot;
		}
	}

	return result.Nomalize();
}

Matrix4x4 AABB::TransMat() const {
	return Matrix4x4::Affine(this->GetRadius(), Vector3::zero, this->GetCentor());
}

AABB AABB::Create(const Vector3 &origin, const Vector3 &radius) {
	AABB result{};
	result.min = -radius;
	result.max = radius;
	result.Swaping();

	return result.AddPos(origin);
}

void AABB::ImGuiDebug(const std::string &group) {
	if (ImGui::TreeNode(group.c_str())) {

		ImGui::DragFloat3("Max", &max.x, 0.1f);
		ImGui::DragFloat3("Min", &min.x, 0.1f);
		Swaping();

		ImGui::TreePop();
	}
}

const AABB &AABB::Swaping() {
	if (min.x > max.x) {
		std::swap(min.x, max.x);
	}
	if (min.y > max.y) {
		std::swap(min.y, max.y);
	}
	if (min.z > max.z) {
		std::swap(min.z, max.z);
	}
	return *this;
}

std::array<Vector3, 8u> AABB::GetVertex() const {
	std::array<Vector3, 8u> result{

		// lower
		Vector3{ min.x, min.y, min.z },
		Vector3{ min.x, min.y, max.z },
		Vector3{ max.x, min.y, max.z },
		Vector3{ max.x, min.y, min.z },
		// higher
		Vector3{ min.x, max.y, min.z },
		Vector3{ min.x, max.y, max.z },
		Vector3{ max.x, max.y, max.z },
		Vector3{ max.x, max.y, min.z },
	};
	return result;
}

void Sphere::ImGuiDebug(const std::string &group) {

	if (ImGui::TreeNode(group.c_str())) {

		ImGui::DragFloat3("Centor", &centor.x, 0.1f);
		ImGui::DragFloat("Radius", &radius, 0.1f);

		ImGui::TreePop();
	}
}

const Matrix4x4 OBB::GetWorldMatrix() const {
	Matrix4x4 result = orientations.MakeRotateMatrix();
	result.GetTranslate() = centor;
	return result;
}

const Matrix4x4 OBB::GetInverseMatrix() const { return GetWorldMatrix().InverseRT(); }

void OBB::ImGuiDebug(const std::string &group, Vector3 &rotate) {

	if (ImGui::TreeNode(group.c_str())) {

		ImGui::DragFloat3("Centor", &centor.x, 0.1f);
		ImGui::DragFloat3("Size", &size.x, 0.1f);

		if (ImGui::DragFloat3("Rotate", &rotate.x, Angle::Dig2Rad)) {
			SetRotate(rotate);
		}

		ImGui::TreePop();
	}
}

void OBB::SetRotate(const Vector3 &euler) {
	orientations = Quaternion::Create(euler);

}

Vector3 Spring::GetAcceleration(const Ball &ball) {
	const Vector3 diff = ball.position - anchor;
	const float length = diff.Length();
	if (length) {
		Vector3 direction = diff.Nomalize();
		Vector3 restPosition = anchor + direction * naturalLength;
		Vector3 desplacement = length * (ball.position - restPosition);
		Vector3 restoringForce = -stiffness * desplacement;

		Vector3 dampingForce = -dampingCoefficient * ball.velocity;
		Vector3 force = restoringForce + dampingForce;
		return force / ball.mass;
	}
	return Vector3::zero;
}

void Pendulum::MoveSwing(const Vector3 &kGravity, const float deltaTime) {
	angularAcceleration = kGravity.y / length * std::sin(angle);
	angularVelocity += angularAcceleration * deltaTime;
	angle += angularVelocity * deltaTime;

	angularAcceleration = 0.f;
}

void ConicalPendulum::MoveAngle(const Vector3 &kGravity, const float deltaTime) {
	angularVelocity = std::sqrt(-kGravity.y / (length * std::cos(halfApexAngle)));
	angle += angularVelocity * deltaTime;
	angle = std::fmod(angle, Angle::PI2);
}

Vector3 ConicalPendulum::GetPos() {
	const float radius = std::sin(halfApexAngle) * length;
	const float height = std::cos(halfApexAngle) * length;
	return Vector3{ std::cos(angle) * radius, -height, -std::sin(angle) * radius } + anchor;
}

void Ball::Update(const Plane &plane, const float deltaTime, const float elasticity) {
	velocity += acceleration * deltaTime;
	if (velocity * plane.normal < 0.f) {
		Capsule capsule{};
		capsule.segment = LineBase{ .origin = position, .diff = velocity, .lineType = LineBase::LineType::Segment };
		capsule.radius = radius;
		if (Collision::IsHit(capsule, plane)) {
			const Vector3 hitPos = capsule.GetHitPoint(plane);
			position = hitPos;
			velocity = velocity.Reflect(plane.normal) * elasticity;
			//* (1.f - (hitPos - capsule.segment.origin).Length() / capsule.segment.diff.Length());
		}
	}
	// else acceleration = Vector3::zero();
	position += velocity * deltaTime;
	acceleration = Vector3::zero;
}

Vector3 Capsule::GetHitPoint(const Plane &plane) const {
	// 引き戻す単位
	const Vector3 invDiff = -segment.diff.Nomalize();
	// 引き戻す時の係数
	const float dot = plane.normal * invDiff;
	// 戻す量
	const Vector3 back = invDiff * (radius / dot);
	return Collision::HitPoint(segment, plane) + back;
}

AABB operator+(const AABB &first, const AABB &second) {
	AABB mergedAABB;
	for (uint32_t i = 0u; i < 3; i++) {
		(&mergedAABB.min.x)[i] = (std::min)((&first.min.x)[i], (&second.min.x)[i]);

		(&mergedAABB.max.x)[i] = (std::max)((&first.max.x)[i], (&second.max.x)[i]);

	}

	//mergedAABB.min.x = (std::min)(first.min.x, second.min.x);
	//mergedAABB.min.y = (std::min)(first.min.y, second.min.y);
	//mergedAABB.min.z = (std::min)(first.min.z, second.min.z);
	//mergedAABB.max.x = (std::max)(first.max.x, second.max.x);
	//mergedAABB.max.y = (std::max)(first.max.y, second.max.y);
	//mergedAABB.max.z = (std::max)(first.max.z, second.max.z);
	return mergedAABB;
}