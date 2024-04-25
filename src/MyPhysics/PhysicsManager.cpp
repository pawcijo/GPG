#include "PhysicsManager.h"

#include <App.h>

#include <cstdio>

PhysicsManager::PhysicsManager(float gravityX, float gravityY, float gravityZ)
    : gravity(gravityX, gravityY, gravityZ) {}

void PhysicsManager::AddOBB(Obb *obb)
{
    obbs.push_back(obb);
}

void PhysicsManager::ResolveCollision(Obb *one, Obb *other)
{
    // Ensure both pointers are valid
    if (!one || !other)
        return;

    // Calculate separation vector between the centers of the two Obbs
    glm::vec3 separationVector = other->GetCenter() - one->GetCenter();

    // Calculate overlap along each axis
    float xOverlap = (one->GetScale().x + other->GetScale().x) / 2.0f - glm::abs(glm::dot(separationVector, glm::vec3(one->GetRotation()[0]))); // Projection along x-axis
    float yOverlap = (one->GetScale().y + other->GetScale().y) / 2.0f - glm::abs(glm::dot(separationVector, glm::vec3(one->GetRotation()[1]))); // Projection along y-axis
    float zOverlap = (one->GetScale().z + other->GetScale().z) / 2.0f - glm::abs(glm::dot(separationVector, glm::vec3(one->GetRotation()[2]))); // Projection along z-axis

    // Find the axis of least overlap
    if (xOverlap < yOverlap && xOverlap < zOverlap)
    {
        // Resolve collision along the x-axis
        float moveX = xOverlap * (glm::dot(separationVector, glm::vec3(one->GetRotation()[0])) < 0 ? -1.0f : 1.0f);
        one->GetCenter() += one->GetRotation()[0] * moveX;

        // Calculate velocity along the x-axis
        glm::vec3 velocityOnAxis = glm::dot(one->GetVelocity(), glm::vec3(one->GetRotation()[0])) * glm::vec3(one->GetRotation()[0]);
        // Adjust velocity along the x-axis
        one->GetVelocity() -= velocityOnAxis;
    }
    else if (yOverlap < xOverlap && yOverlap < zOverlap)
    {
        // Resolve collision along the y-axis
        float moveY = yOverlap * (glm::dot(separationVector, glm::vec3(one->GetRotation()[1])) < 0 ? -1.0f : 1.0f);
        one->GetCenter() += one->GetRotation()[1] * moveY;

        // Calculate velocity along the y-axis
        glm::vec3 velocityOnAxis = glm::dot(one->GetVelocity(), glm::vec3(one->GetRotation()[1])) * glm::vec3(one->GetRotation()[1]);
        // Adjust velocity along the y-axis
        one->GetVelocity() -= velocityOnAxis;
    }
    else
    {
        // Resolve collision along the z-axis
        float moveZ = zOverlap * (glm::dot(separationVector, glm::vec3(one->GetRotation()[2])) < 0 ? -1.0f : 1.0f);
        one->GetCenter() += one->GetRotation()[2] * moveZ;

        // Calculate velocity along the z-axis
        glm::vec3 velocityOnAxis = glm::dot(one->GetVelocity(), glm::vec3(one->GetRotation()[2])) * glm::vec3(one->GetRotation()[2]);
        // Adjust velocity along the z-axis
        one->GetVelocity() -= velocityOnAxis;
    }
}

bool PhysicsManager::Intersects(Obb *obb1, Obb *obb2)
{
    /*
    if (!obb1 || !obb2)
        return false;

    // Calculate the separation vector between the centers of the two OBBs
    glm::vec3 separationVector = obb2->GetCenter() - obb1->GetCenter();

    // Calculate the axis vectors for each OBB
    glm::vec3 obb1Axes[3] = {
        glm::normalize(obb1->GetRotation()[0]),
        glm::normalize(obb1->GetRotation()[1]),
        glm::normalize(obb1->GetRotation()[2])};
    glm::vec3 obb2Axes[3] = {
        glm::normalize(obb2->GetRotation()[0]),
        glm::normalize(obb2->GetRotation()[1]),
        glm::normalize(obb2->GetRotation()[2])};

    // Calculate the extent vectors for each OBB
    glm::vec3 obb1Extents = obb1->GetScale() / 2.0f;
    glm::vec3 obb2Extents = obb2->GetScale() / 2.0f;

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

    */

   return false;
}

void PhysicsManager::ApplyPositionCorrection(Obb *obb)
{
    // Implementation depends on how you manage corrections, an example might include:
    if (!obb->IsStatic() && obb->needsCorrection)
    {
        // Apply some calculated correction vector to the position
        obb->SetCenter(obb->GetCenter() + obb->correctionVector);
        // Reset correction flag and vector for next update
        obb->needsCorrection = false;
        obb->correctionVector = glm::vec3(0);
    }
}

void PhysicsManager::ResolveAllCollision()
{
    for (auto collisionPari : collisionSet)
    {
        ResolveCollision(collisionPari.first, collisionPari.second);
    }
}

void PhysicsManager::Update(float deltaTime)
{

    deltaTime = 0.016;
    for (auto &obb : obbs)
    {
        if (!obb->IsStatic())
        {
            // Apply gravity to velocity
            obb->SetVelocity(obb->GetVelocity() + (gravity * deltaTime * 0.99f));

            // Update position based on new velocity
            obb->SetCenter(obb->GetCenter() + (obb->GetVelocity() * deltaTime));
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
            if (Intersects(obbs[i], obbs[j]))
            {
                auto collisionPair = std::pair{obbs[i], obbs[j]};
                if (collisionSet.find(collisionPair) == collisionSet.end())
                {
                    collisionSet.insert(collisionPair);
                }
            }
        }
    }

    // TODO SET Collision RESOLVE STEP int
    for (int i = 0; i < 5; i++)
    {
        ResolveAllCollision();
    }

    // Update Objects position
}

glm::vec3 rotationFromMatrix(const glm::mat3 &matrix)
{
    float angleX = atan2(matrix[2][1], matrix[2][2]);
    float angleY = atan2(-matrix[2][0], sqrt(matrix[2][1] * matrix[2][1] + matrix[2][2] * matrix[2][2]));
    float angleZ = atan2(matrix[1][0], matrix[0][0]);

    return glm::vec3(angleX, angleY, angleZ);
}

void PhysicsManager::SynchroniseWithRender()
{
    if (nullptr != app)
    {
        for (int i = 0; i < app->mBoxes.size(); i++)
        {
            if (nullptr != app->mBoxes[i]->GetObb() && nullptr != obbs[i])
            {
                app->mBoxes[i]->getTransform().setPosition(obbs[i]->GetCenter());
                app->mBoxes[i]->getTransform().setRotation(obbs[i]->GetRotation());
            }
        }
    }
}
