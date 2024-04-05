#pragma once

#include <glm/glm.hpp>

#include <vector>

struct CollisionResult
{
    bool colliding;
    glm::vec3 normal;
    float depth;
    std::vector<glm::vec3> contacts;
};

glm::vec3 MultiplyVector(const glm::vec3& vec, const glm::mat4& mat);
glm::vec3 MultiplyVector(const glm::vec3& vec, const glm::mat3& mat);



class AABB{

public:
    glm::vec3 position;
    glm::vec3 size;//half_size

    inline AABB() : size(1,1,1){ }
    inline AABB(const glm::vec3& aPoint, const glm::vec3& aSize):
    position(aPoint),
    size(aSize){}
};

glm::vec3 GetMin(const AABB& aabb);
glm::vec3 GetMax(const AABB& aabb);
AABB FromMinMax(const glm::vec3& min, const glm::vec3& max);

bool PointInAABB(const glm::vec3& point, const AABB& aabb);
glm::vec3 ClosestPoint(const AABB& aabb, const glm::vec3& point);

bool ContainsPoint(const AABB& aabb, const glm::vec3& point);
bool ContainsPoint(const glm::vec3& point, const AABB& aabb);

glm::vec3 ClosestPoint(const glm::vec3& point, const AABB& aabb);

bool AABBAABB(const AABB& aabb1, const AABB& aabb2);


