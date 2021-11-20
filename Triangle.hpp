#include "glm/glm.hpp"

#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

struct Triangle {
	glm::vec3 a, b, c, o, n_a, n_b, n_c;

	Triangle();
	Triangle(const Triangle& other) = default;
	Triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c) : a(a), b(b), c(c) {
		o = (a + b + c) / 3.0f;
	}
	Triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 n_a, glm::vec3 n_b, glm::vec3 n_c) : Triangle(a, b, c) {
		n_a = n_a;
		n_b = n_b;
		n_c = n_c;
	}

};

#endif