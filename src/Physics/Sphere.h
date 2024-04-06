#pragma once

#include <glm/glm.hpp>

class Sphere
{

public:
	glm::vec3 position;
	float radius;

	inline Sphere() : radius(1.0f), position(0.0f, 0.0f, 0.0f) {}
	inline Sphere(const glm::vec3 &p, float r) : position(p), radius(r) {}
};
