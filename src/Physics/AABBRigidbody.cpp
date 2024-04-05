#include "AABBRigidbody.h"


glm::mat4 AABBRigidBody::InvTensor() {
	if (mass == 0) {
		return glm::mat4(
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0
		);
	}
	float ix = 0.0f;
	float iy = 0.0f;
	float iz = 0.0f;
	float iw = 0.0f;

	if (mass != 0 && type == Sphere) {
		/*float r2 = sphere.radius * sphere.radius;
		float fraction = (2.0f / 5.0f);

		ix = r2 * mass * fraction;
		iy = r2 * mass * fraction;
		iz = r2 * mass * fraction;
		iw = 1.0f;
        */
	}
	else if (mass != 0 && type == Box) {
		glm::vec3 size = box.size * 2.0f;
		float fraction = (1.0f / 12.0f);

		float x2 = size.x * size.x;
		float y2 = size.y * size.y;
		float z2 = size.z * size.z;

		ix = (y2 + z2) * mass * fraction;
		iy = (x2 + z2) * mass * fraction;
		iz = (x2 + y2) * mass * fraction;
		iw = 1.0f;
	}

	return glm::inverse(glm::mat4(
		ix, 0, 0, 0,
		0, iy, 0, 0,
		0, 0, iz, 0,
		0, 0, 0, iw));
}


void AABBRigidBody::ApplyForces() {
	forces = GRAVITY_CONST * mass;
}

void  AABBRigidBody::AddRotationalImpulse(const glm::vec3& point, const glm::vec3& impulse) {
	glm::vec3 centerOfMass = position;
	glm::vec3 torque = glm::cross(point - centerOfMass, impulse);

	glm::vec3 angAccel = MultiplyVector(torque, InvTensor());
	angVel = angVel + angAccel;
}