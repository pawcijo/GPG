#include "Obb.h"
#include <cmath>
#include <algorithm>

Obb::Obb()
    : center(0.0f), scale(1.0f), rotation(1.0f), mass(1.0f), velocity(0.0f), isStatic(false) {}

Obb::Obb(const Obb& other)
    : center(other.center), scale(other.scale), rotation(other.rotation), mass(other.mass), velocity(other.velocity), isStatic(other.isStatic) {}

Obb::Obb(glm::vec3 newCenter, glm::vec3 newScale, glm::mat3 newRotation, float newMass, glm::vec3 initialVelocity, bool staticObj /* = false */) 
    : center(newCenter), scale(newScale), rotation(newRotation), mass(newMass), velocity(initialVelocity), isStatic(staticObj) {}



bool Obb::intersects(const Obb& other) const {
    glm::vec3 t = other.center - center; // Vector from this center to other center in world space

    // Transform t into this object's local coordinate system
    glm::vec3 tLocal(
        glm::dot(t, glm::vec3(rotation[0])),
        glm::dot(t, glm::vec3(rotation[1])),
        glm::dot(t, glm::vec3(rotation[2]))
    );

    // Compute the absolute differences between the two objects' centers in local space
    glm::vec3 diff(
        abs(tLocal.x) - (scale.x + other.scale.x) / 2.0f,
        abs(tLocal.y) - (scale.y + other.scale.y) / 2.0f,
        abs(tLocal.z) - (scale.z + other.scale.z) / 2.0f
    );

    // Check if the objects overlap along each axis
    if (diff.x < 0 && diff.y < 0 && diff.z < 0) {
        return true; // Overlapping along all axes means intersection
    }

    return false;
}

glm::mat4 Obb::transform() const {
    glm::mat4 translation = glm::translate(glm::mat4(1.0f), center);
    glm::mat4 rotationMat = glm::mat4(rotation);
    glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), scale); // Include scale transformation
    return translation * rotationMat * scaleMat; // Apply scale transformation
}


void Obb::resolveCollision(Obb& other) {
    // Move the objects apart along the axis of least penetration
    glm::vec3 normal = glm::normalize(center - other.center);
    float penetration = glm::length(center - other.center) - (scale.x + other.scale.x) / 2.0f;
    glm::vec3 separation = normal * penetration;

    // Move this object
    center += separation / 2.0f;

    // Move the other object
    other.center -= separation / 2.0f;
}