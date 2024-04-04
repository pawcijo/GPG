#include "AABB.h"

#include <cmath>
#include <cfloat>


#define CMP(x, y) (fabsf(x - y) <= FLT_EPSILON * fmaxf(1.0f, fmaxf(fabsf(x), fabsf(y))))

glm::vec3 GetMin(const AABB& aabb) {
	glm::vec3 p1 = aabb.position + aabb.size;
	glm::vec3 p2 = aabb.position - aabb.size;

	return glm::vec3(fminf(p1.x, p2.x), fminf(p1.y, p2.y), fminf(p1.z, p2.z));
}
glm::vec3 GetMax(const AABB& aabb) {
	glm::vec3 p1 = aabb.position + aabb.size;
	glm::vec3 p2 = aabb.position - aabb.size;

	return glm::vec3(fmaxf(p1.x, p2.x), fmaxf(p1.y, p2.y), fmaxf(p1.z, p2.z));
}


AABB FromMinMax(const glm::vec3& min, const glm::vec3& max) {
	return AABB((min + max) * 0.5f, (max - min) * 0.5f);
}

bool PointInAABB(const glm::vec3& point, const AABB& aabb) {
	glm::vec3 min = GetMin(aabb);
	glm::vec3 max = GetMax(aabb);

	if (point.x < min.x || point.y < min.y || point.z < min.z) {
		return false;
	}
	if (point.x > max.x || point.y > max.y || point.z > max.z) {
		return false;
	}

	return true;
}

glm::vec3 ClosestPoint(const AABB& aabb, const glm::vec3& point) {
	glm::vec3 result = point;
	glm::vec3 min = GetMin(aabb);
	glm::vec3 max = GetMax(aabb);

	result.x = (result.x < min.x) ? min.x : result.x;
	result.y = (result.y < min.x) ? min.y : result.y;
	result.z = (result.z < min.x) ? min.z : result.z;

	result.x = (result.x > max.x) ? max.x : result.x;
	result.y = (result.y > max.x) ? max.y : result.y;
	result.z = (result.z > max.x) ? max.z : result.z;

	return result;
}

bool ContainsPoint(const AABB& aabb, const glm::vec3& point) {
	return PointInAABB(point, aabb);
}

bool ContainsPoint(const glm::vec3& point, const AABB& aabb) {
	return PointInAABB(point, aabb);
}

glm::vec3 ClosestPoint(const glm::vec3& point, const AABB& aabb) {
	return ClosestPoint(aabb, point);
}

bool AABBAABB(const AABB& aabb1, const AABB& aabb2) {
	glm::vec3 aMin = GetMin(aabb1);
	glm::vec3 aMax = GetMax(aabb1);
	glm::vec3 bMin = GetMin(aabb2);
	glm::vec3 bMax = GetMax(aabb2);

	return	(aMin.x <= bMax.x && aMax.x >= bMin.x) &&
			(aMin.y <= bMax.y && aMax.y >= bMin.y) &&
			(aMin.z <= bMax.z && aMax.z >= bMin.z);
}
