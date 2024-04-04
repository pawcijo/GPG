#include <PhysicsManager.h>

void ResetCollisionResult(CollisionResult* result) {
	if (result != 0) {
		result->colliding = false;
		result->normal = glm::vec3(0, 0, 1);
		result->depth = FLT_MAX;
		if (result->contacts.size() > 0) {
			result->contacts.clear();
		}
	}
}


PhysicsManager::PhysicsManager()
{
    colliders1.reserve(100);
    colliders2.reserve(100);
    results.reserve(100);
}

void PhysicsManager::Update(float deltaTime)
{
}