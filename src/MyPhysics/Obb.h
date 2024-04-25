#pragma once

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cstdio>

#include <Transform.h>

class Box;

class Obb
{
    // Parent transform, could be seperate from parent but now it should be the same
    Transform &transform;

    float mass;
    glm::vec3 velocity;
    bool isStatic; // Flag indicating whether the object is static

public:
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

    Obb(const Obb &other);
    Obb(Transform &transform, float newMass, glm::vec3 initialVelocity, bool staticObj = false);

    glm::vec3 GetCenter() { return transform.getPosition(); };
    glm::vec3 GetScale() { return transform.getScale(); };
    glm::vec3 GetRotation() { return transform.getRotation(); };

    glm::vec3 &GetVelocity() { return velocity; }

    void SetCenter(glm::vec3 aCenter) { transform.setPosition(aCenter); };
    void SetScale(glm::vec3 aScale) { transform.setScale(aScale); };
    void SetRotation(glm::vec3 aRotation) { transform.setRotation(aRotation); };

    void Translate(glm::vec3 aValue) { transform.translate(aValue); }

    void Rotate(float aValue, glm::vec3 axis) { transform.rotate(aValue, axis); };

    void SetVelocity(glm::vec3 aVelocity) { velocity = aVelocity; }

    bool IsStatic() { return isStatic; }

    bool operator==(const Obb &p) const;

    Transform GetTransform() const;
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
