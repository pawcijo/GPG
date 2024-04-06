#pragma once
#include <glm/glm.hpp>


struct Plane {
	glm::vec3 normal;
	float distance;

	inline Plane() : normal(1, 0, 0),distance(0) { }
	inline Plane(const glm::vec3& n, float d) :
		normal(n), distance(d) { }
};