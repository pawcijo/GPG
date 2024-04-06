#include "PhysicsManager.h"

#include "RigidbodyImpl.h"

#include <cstdio>

PhysicsManager::PhysicsManager()
{
	LinearProjectionPercent = 0.45f;
	PenetrationSlack = 0.01f;
	ImpulseIteration = 5;

	colliders1.reserve(100);
	colliders2.reserve(100);
	results.reserve(100);
}

void PhysicsManager::Update(float deltaTime)
{

	colliders1.clear();
	colliders2.clear();
	results.clear();

	// find colliding objects
	{
		CollisionResult result;
		for (int i = 0, size = bodies.size(); i < size; ++i)
		{
			for (int j = i; j < size; ++j)
			{
				if (i == j)
				{
					continue;
				}
				ResetCollisionResult(&result);
				if (bodies[i]->HasVolume() && bodies[j]->HasVolume())
				{
					RigidbodyImpl *m1 = (RigidbodyImpl *)bodies[i];
					RigidbodyImpl *m2 = (RigidbodyImpl *)bodies[j];
					result = FindCollisionFeatures(*m1, *m2);
				}
				if (result.colliding)
				{
#if 0 
					bool isDuplicate = false;
					for (int k = 0, kSize = colliders1.size(); k < kSize; ++k) {
						if (colliders1[k] == bodies[i] || colliders1[k] == bodies[j]) {
							if (colliders2[k] == bodies[i] || colliders2[k] == bodies[j]) {
								isDuplicate = true;
								break;
							}
						}
					}

					if (!isDuplicate) {
						for (int k = 0, kSize = colliders2.size(); k < kSize; ++k) {
							if (colliders2[k] == bodies[i] || colliders2[k] == bodies[j]) {
								if (colliders1[k] == bodies[i] || colliders1[k] == bodies[j]) {
									isDuplicate = true;
									break;
								}
							}
						}
					}
					if (!isDuplicate)
#endif

					{
						colliders1.push_back(bodies[i]);
						colliders2.push_back(bodies[j]);
						results.push_back(result);
					}
				}
			}
		}
	}

	// Apply forces on the physical bodies
	{

		// Calculate foces acting on the object
		for (int i = 0, size = bodies.size(); i < size; ++i)
		{
			bodies[i]->ApplyForces();
		}
	}

	// Apply impulses to resolve collisions
	{
		for (int k = 0; k < ImpulseIteration; ++k)
		{ // Apply impulses
			for (int i = 0, size = results.size(); i < size; ++i)
			{
				for (int j = 0, jSize = results[i].contacts.size(); j < jSize; ++j)
				{
					if (colliders1[i]->HasVolume() && colliders2[i]->HasVolume())
					{
						RigidbodyImpl *m1 = (RigidbodyImpl *)colliders1[i];
						RigidbodyImpl *m2 = (RigidbodyImpl *)colliders2[i];
						ApplyImpulse(*m1, *m2, results[i], j);
					}
				}
			}
		}
	}

	// Integrate velocity and impulse of objects
	{
		for (int i = 0, size = bodies.size(); i < size; ++i)
		{
			bodies[i]->Update(deltaTime);
		}
	}

	// Correct position to avoid sinking
	{
		if (DoLinearProjection)
		{
			for (int i = 0, size = results.size(); i < size; ++i)
			{
				if (!colliders1[i]->HasVolume() && !colliders2[i]->HasVolume())
				{
					continue;
				}

				RigidbodyImpl *m1 = (RigidbodyImpl *)colliders1[i];
				RigidbodyImpl *m2 = (RigidbodyImpl *)colliders2[i];
				float totalMass = m1->InvMass() + m2->InvMass();

				if (totalMass == 0.0f)
				{
					continue;
				}

				float depth = fmaxf(results[i].depth - PenetrationSlack, 0.0f);
				float scalar = (totalMass == 0.0f) ? 0.0f : depth / totalMass;
				glm::vec3 correction = results[i].normal * scalar * LinearProjectionPercent;

				m1->position = m1->position - correction * m1->InvMass();
				m2->position = m2->position + correction * m2->InvMass();

				m1->SynchCollisionVolumes();
				m2->SynchCollisionVolumes();
			}
		}
	}
	//  ready to render
}

void PhysicsManager::AddRigidbody(Rigidbody *body)
{
	bodies.push_back(body);
}

void PhysicsManager::AddConstraint(const OBB &obb)
{
	constraints.push_back(obb);
}

void PhysicsManager::ClearRigidbodys()
{
	bodies.clear();
}

void PhysicsManager::ClearConstraints()
{
	constraints.clear();
}