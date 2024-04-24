#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cstdio>

class Box;

class Obb
{
public:
    glm::vec3 center;
    glm::vec3 scale; // Changed parameter name from extents to scale
    glm::mat3 rotation;

    float mass;
    glm::vec3 velocity;
    bool isStatic; // Flag indicating whether the object is static

    bool intersect = false;

    Box *object = nullptr;

    bool needsCorrection = false;              // Flag to indicate correction is needed
    glm::vec3 correctionVector = glm::vec3(0); // Vector to apply corrections

    float InvMass()
    {
        if (mass == 0.0f)
        {
            return 0.0f;
        }
        return 1.0f / mass;
    }

    Obb();
    Obb(const Obb &other);
    Obb(glm::vec3 newCenter, glm::vec3 newScale, glm::mat3 newRotation, float newMass, glm::vec3 initialVelocity, bool staticObj = false);

    bool operator==(const Obb &p) const;

    glm::mat4 transform() const;
};

std::size_t GetHash(const std::pair<Obb *, Obb *> &pairObb);

class PairObbHash
{
public:
    std::size_t operator()(const std::pair<Obb *, Obb *> &pairObb) const
    {
        // Hash the ids of the two Obb objects in the pair
        if (nullptr == pairObb.first->object || nullptr == pairObb.second->object)
        {
            printf("Brawo brawo, BRAWO\n");
        }
        return GetHash(pairObb);
    }
};
