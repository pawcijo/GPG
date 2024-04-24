#include "Obb.h"
#include <cmath>
#include <algorithm>

#include <SimpleShape/Box.h>

Obb::Obb()
    : center(0.0f), scale(1.0f), rotation(1.0f), mass(1.0f), velocity(0.0f), isStatic(false) {}

Obb::Obb(const Obb &other)
    : center(other.center), scale(other.scale), rotation(other.rotation), mass(other.mass), velocity(other.velocity), isStatic(other.isStatic) {}

Obb::Obb(glm::vec3 newCenter, glm::vec3 newScale, glm::mat3 newRotation, float newMass, glm::vec3 initialVelocity, bool staticObj /* = false */)
    : center(newCenter), scale(newScale), rotation(newRotation), mass(newMass), velocity(initialVelocity), isStatic(staticObj) {}

glm::mat4 Obb::transform() const
{
    glm::mat4 translation = glm::translate(glm::mat4(1.0f), center);
    glm::mat4 rotationMat = glm::mat4(rotation);
    glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), scale); // Include scale transformation
    return translation * rotationMat * scaleMat;             // Apply scale transformation
}

  bool Obb::operator==(const Obb& p) const{
        return this->object->ObjectId() == p.object->ObjectId();
    }

    std::size_t GetHash(const std::pair<Obb *, Obb *> &pairObb)
{
    std::size_t hash1 = std::hash<unsigned int>()(pairObb.first->object->ObjectId());
    std::size_t hash2 = std::hash<unsigned int>()(pairObb.second->object->ObjectId());
    return hash1 ^ (hash2 + 0x9e3779b9 + (hash1 << 6) + (hash1 >> 2));
}

