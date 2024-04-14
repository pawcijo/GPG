#include "PhysicsManager.h"

#include <cstdio>

PhysicsManager::PhysicsManager(float gravityX, float gravityY, float gravityZ)
    : gravity(gravityX, gravityY, gravityZ) {}

void PhysicsManager::addOBB(const Obb &obb)
{
    obbs.push_back(obb);
}

void PhysicsManager::update(float deltaTime) {
    for (auto& obb : obbs) {
        if (!obb.isStatic) {
            // Apply gravity to velocity
            obb.velocity += gravity * deltaTime;

            // Update position based on new velocity
            obb.center += obb.velocity * deltaTime;
        }
    }

    // Check and resolve collisions
    for (size_t i = 0; i < obbs.size(); ++i) {
        for (size_t j = i + 1; j < obbs.size(); ++j) {
            if (obbs[i].intersects(obbs[j])) {
                printf("Duuupsko \n");
                obbs[i].resolveCollision(obbs[j]);
            }
        }
    }
}

