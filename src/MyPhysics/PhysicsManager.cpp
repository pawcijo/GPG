#include "PhysicsManager.h"

#include <cstdio>

PhysicsManager::PhysicsManager(float gravityX, float gravityY, float gravityZ)
    : gravity(gravityX, gravityY, gravityZ) {}

void PhysicsManager::addOBB(Obb *obb)
{
    obbs.push_back(obb);
}

float calculatePenetrationDepth(const Obb *one, const Obb *other, const glm::vec3 &axis)
{
    // Calculate the projection radius of the first OBB on the axis
    float radiusOne = one->scale.x * std::abs(glm::dot(axis, one->rotation[0])) +
                      one->scale.y * std::abs(glm::dot(axis, one->rotation[1])) +
                      one->scale.z * std::abs(glm::dot(axis, one->rotation[2]));

    // Calculate the projection radius of the second OBB on the axis
    float radiusOther = other->scale.x * std::abs(glm::dot(axis, other->rotation[0])) +
                        other->scale.y * std::abs(glm::dot(axis, other->rotation[1])) +
                        other->scale.z * std::abs(glm::dot(axis, other->rotation[2]));

    // Calculate the distance between the centers projected onto the axis
    glm::vec3 centerVec = other->center - one->center;
    float distance = std::abs(glm::dot(centerVec, axis));

    // Calculate the penetration depth along this axis
    float penetrationDepth = radiusOne + radiusOther - distance;

    return penetrationDepth > 0 ? penetrationDepth : 0.0f;
}

void PhysicsManager::resolveCollision(Obb *one, Obb *other)
{
    // Ensure both pointers are valid
    if (!one || !other)
        return;

    // Calculate separation vector between the centers of the two Obbs
    glm::vec3 separationVector = other->center - one->center;

    // Calculate overlap along each axis
    float xOverlap = (one->scale.x + other->scale.x) / 2.0f - glm::abs(glm::dot(separationVector, glm::vec3(one->rotation[0]))); // Projection along x-axis
    float yOverlap = (one->scale.y + other->scale.y) / 2.0f - glm::abs(glm::dot(separationVector, glm::vec3(one->rotation[1]))); // Projection along y-axis
    float zOverlap = (one->scale.z + other->scale.z) / 2.0f - glm::abs(glm::dot(separationVector, glm::vec3(one->rotation[2]))); // Projection along z-axis

    // Find the axis of least overlap
    if (xOverlap < yOverlap && xOverlap < zOverlap)
    {
        // Resolve collision along the x-axis
        float moveX = xOverlap * (glm::dot(separationVector, glm::vec3(one->rotation[0])) < 0 ? -1.0f : 1.0f);
        one->center += one->rotation[0] * moveX;

        // Calculate velocity along the x-axis
        glm::vec3 velocityOnAxis = glm::dot(one->velocity, glm::vec3(one->rotation[0])) * glm::vec3(one->rotation[0]);
        // Adjust velocity along the x-axis
        one->velocity -= velocityOnAxis;
    }
    else if (yOverlap < xOverlap && yOverlap < zOverlap)
    {
        // Resolve collision along the y-axis
        float moveY = yOverlap * (glm::dot(separationVector, glm::vec3(one->rotation[1])) < 0 ? -1.0f : 1.0f);
        one->center += one->rotation[1] * moveY;

        // Calculate velocity along the y-axis
        glm::vec3 velocityOnAxis = glm::dot(one->velocity, glm::vec3(one->rotation[1])) * glm::vec3(one->rotation[1]);
        // Adjust velocity along the y-axis
        one->velocity -= velocityOnAxis;
    }
    else
    {
        // Resolve collision along the z-axis
        float moveZ = zOverlap * (glm::dot(separationVector, glm::vec3(one->rotation[2])) < 0 ? -1.0f : 1.0f);
        one->center += one->rotation[2] * moveZ;

        // Calculate velocity along the z-axis
        glm::vec3 velocityOnAxis = glm::dot(one->velocity, glm::vec3(one->rotation[2])) * glm::vec3(one->rotation[2]);
        // Adjust velocity along the z-axis
        one->velocity -= velocityOnAxis;
    }
}

bool PhysicsManager::intersects(Obb *obb1, Obb *obb2)
{
    if (!obb1 || !obb2)
        return false;

    // Calculate the separation vector between the centers of the two OBBs
    glm::vec3 separationVector = obb2->center - obb1->center;

    // Calculate the axis vectors for each OBB
    glm::vec3 obb1Axes[3] = {
        glm::normalize(obb1->rotation[0]),
        glm::normalize(obb1->rotation[1]),
        glm::normalize(obb1->rotation[2])};
    glm::vec3 obb2Axes[3] = {
        glm::normalize(obb2->rotation[0]),
        glm::normalize(obb2->rotation[1]),
        glm::normalize(obb2->rotation[2])};

    // Calculate the extent vectors for each OBB
    glm::vec3 obb1Extents = obb1->scale / 2.0f;
    glm::vec3 obb2Extents = obb2->scale / 2.0f;

    // Calculate the separation along each axis
    for (int i = 0; i < 3; ++i)
    {
        float distance = glm::abs(glm::dot(separationVector, obb1Axes[i]));
        float obb1ProjectedExtent = glm::dot(obb1Extents, glm::abs(obb1Axes[i]));
        float obb2ProjectedExtent = glm::dot(obb2Extents, glm::abs(obb1Axes[i]));

        if (distance > obb1ProjectedExtent + obb2ProjectedExtent)
        {
            // No overlap on this axis, so the OBBs do not intersect
            return false;
        }
    }

    for (int i = 0; i < 3; ++i)
    {
        float distance = glm::abs(glm::dot(separationVector, obb2Axes[i]));
        float obb1ProjectedExtent = glm::dot(obb1Extents, glm::abs(obb2Axes[i]));
        float obb2ProjectedExtent = glm::dot(obb2Extents, glm::abs(obb2Axes[i]));

        if (distance > obb1ProjectedExtent + obb2ProjectedExtent)
        {
            // No overlap on this axis, so the OBBs do not intersect
            return false;
        }
    }

    // Overlap exists along all axes, so the OBBs intersect
    return true;
}

void PhysicsManager::applyPositionCorrection(Obb *obb)
{
    // Implementation depends on how you manage corrections, an example might include:
    if (!obb->isStatic && obb->needsCorrection)
    {
        // Apply some calculated correction vector to the position
        obb->center += obb->correctionVector;
        // Reset correction flag and vector for next update
        obb->needsCorrection = false;
        obb->correctionVector = glm::vec3(0);
    }
}

void PhysicsManager::update(float deltaTime)
{

    // deltaTime = 0.016;
    for (auto &obb : obbs)
    {
        if (!obb->isStatic)
        {
            // Apply gravity to velocity
            obb->velocity += gravity * deltaTime * 0.99f;

            // Update position based on new velocity
            obb->center += obb->velocity * deltaTime;
        }
    }

    // Check and resolve collisions
    for (size_t i = 0; i < obbs.size(); ++i)
    {
        for (size_t j = i + 1; j < obbs.size(); ++j)
        {
            if (i == j)
            {
                continue;
            }
            if (intersects(obbs[i], obbs[j]))
            {
                auto collisionPair = std::pair{obbs[i], obbs[j]};
                if (collisionMap.find(collisionPair) == collisionMap.end())
                {
                    collisionMap.insert(collisionPair);
                }
            }
        }
    }

    for (auto &obb : obbs)
    {
        // applyPositionCorrection(obb);
    }
}
