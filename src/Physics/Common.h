#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "Sphere.h"
#include "AABB.h"
#include "OBB.h"
#include "Triangle.h"
#include "Line.h"
#include "Plane.h"
#include "Ray.h"

struct Interval
{
    float min;
    float max;
};


float RAD2DEG(float radians);
float DEG2RAD(float degrees);
float CorrectDegrees(float degrees);

struct CollisionResult
{
    bool colliding;
    glm::vec3 normal;
    float depth;
    std::vector<glm::vec3> contacts;
};

void ResetCollisionResult(CollisionResult *result);

glm::vec3 MultiplyVector(const glm::vec3 &vec, const glm::mat4 &mat);
glm::vec3 MultiplyVector(const glm::vec3 &vec, const glm::mat3 &mat);

float Magnitude(const glm::vec2 &v);
float Magnitude(const glm::vec3 &v);

float MagnitudeSq(const glm::vec2 &v);
float MagnitudeSq(const glm::vec3 &v);

CollisionResult FindCollisionFeatures(const OBB &A, const Sphere &B);
CollisionResult FindCollisionFeatures(const OBB &ra,const OBB &rb);
CollisionResult FindCollisionFeatures(const Sphere &A, const Sphere &B);

glm::mat4 Rotation(float pitch, float yaw, float roll);    // X, Y, Z
glm::mat3 Rotation3x3(float pitch, float yaw, float roll); // X, Y, Z
glm::mat2 Rotation2x2(float angle);
glm::mat4 YawPitchRoll(float yaw, float pitch, float roll); // Y, X, Z

glm::mat4 XRotation(float angle);
glm::mat3 XRotation3x3(float angle);

glm::mat4 YRotation(float angle);
glm::mat3 YRotation3x3(float angle);

glm::mat4 ZRotation(float angle);
glm::mat3 ZRotation3x3(float angle);

float PenetrationDepth(const OBB &o1, const OBB &o2, const glm::vec3 &axis, bool *outShouldFlip);

bool SphereSphere(const Sphere &s1, const Sphere &s2);
bool SphereAABB(const Sphere &sphere, const AABB &aabb);
bool SphereOBB(const Sphere &sphere, const OBB &obb);
bool AABBAABB(const AABB &aabb1, const AABB &aabb2);
bool AABBOBB(const AABB &aabb, const OBB &obb);
bool OBBOBB(const OBB &obb1, const OBB &obb2);

bool PointInSphere(const glm::vec3& point, const Sphere& sphere);
bool PointInAABB(const glm::vec3& point, const AABB& aabb);
bool PointInOBB(const glm::vec3& point, const OBB& obb);

Interval GetInterval(const AABB &aabb, const glm::vec3 &axis);
Interval GetInterval(const OBB &obb, const glm::vec3 &axis);
Interval GetInterval(const Triangle &triangle, const glm::vec3 &axis);

glm::vec3 GetMin(const AABB &aabb);
glm::vec3 GetMax(const AABB &aabb);
AABB FromMinMax(const glm::vec3 &min, const glm::vec3 &max);

bool PointInAABB(const glm::vec3 &point, const AABB &aabb);


bool ContainsPoint(const AABB &aabb, const glm::vec3 &point);
bool ContainsPoint(const glm::vec3 &point, const AABB &aabb);



CollisionResult FindCollisionFeatures(const AABB &A, const AABB &B);

std::vector<glm::vec3> ClipEdgesToOBB(const std::vector<Line>& edges, const OBB& obb);
std::vector<Plane> GetPlanes(const OBB& obb);
std::vector<Line> GetEdges(const OBB& obb);
Plane FromTriangle(const Triangle& t);
bool PointInTriangle(const glm::vec3& p, const Triangle& t);

bool ClipToPlane(const Plane& plane, const Line& line, glm::vec3* outPoint);
std::vector<glm::vec3> GetVertices(const OBB& obb);

glm::vec3 ClosestPoint(const Sphere& sphere, const glm::vec3& point);
glm::vec3 ClosestPoint(const AABB& aabb, const glm::vec3& point);
glm::vec3 ClosestPoint(const OBB& obb, const glm::vec3& point);
glm::vec3 ClosestPoint(const Plane& plane, const glm::vec3& point);
glm::vec3 ClosestPoint(const Line& line, const glm::vec3& point);
glm::vec3 ClosestPoint(const Ray& ray, const glm::vec3& point);

glm::vec3 ClosestPoint(const glm::vec3& point, const Sphere& sphere);
glm::vec3 ClosestPoint(const glm::vec3& point, const AABB& aabb);
glm::vec3 ClosestPoint(const glm::vec3& point, const OBB& obb);
glm::vec3 ClosestPoint(const glm::vec3& point, const Plane& plane);
glm::vec3 ClosestPoint(const glm::vec3& point, const Line& line);
glm::vec3 ClosestPoint(const glm::vec3& point, const Ray& ray);
glm::vec3 ClosestPoint(const glm::vec3& p, const Triangle& t);

