#include "Obb.h"
#include <cmath>
#include <algorithm>

#include <Transform.h>

#include <SimpleShape/Box.h>

Obb::Obb(const Obb &other)
    : transform(other.transform), mass(other.mass), velocity(other.velocity), isStatic(other.isStatic) {}

Obb::Obb(Transform &aTransform, float aMass, glm::vec3 aVelocity, bool aStaticObj /*= false*/)
    : transform(aTransform), mass(aMass), velocity(aVelocity), isStatic(aStaticObj) {}

Transform Obb::GetTransform() const
{
    return transform; // Apply scale transformation
}

bool Obb::operator==(const Obb &p) const
{
    return this->object->ObjectId() == p.object->ObjectId();
}

std::size_t GetHash(const std::pair<Obb *, Obb *> &pairObb)
{
    std::size_t hash1 = std::hash<unsigned int>()(pairObb.first->object->ObjectId());
    std::size_t hash2 = std::hash<unsigned int>()(pairObb.second->object->ObjectId());
    return hash1 ^ (hash2 + 0x9e3779b9 + (hash1 << 6) + (hash1 >> 2));
}
