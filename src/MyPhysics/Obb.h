#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Obb {
public:
    glm::vec3 center;
    glm::vec3 scale; // Changed parameter name from extents to scale
    glm::mat3 rotation;
    float mass;
    glm::vec3 velocity;
    bool isStatic; // Flag indicating whether the object is static

    Obb();
    Obb(const Obb& other);
    Obb(glm::vec3 newCenter, glm::vec3 newScale, glm::mat3 newRotation, float newMass, glm::vec3 initialVelocity, bool staticObj = false);

    bool intersects(const Obb& other) const;
    glm::mat4 transform() const;
    void resolveCollision(Obb& other);
};
