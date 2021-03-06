#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#include "glm/glm.hpp"
#include "Object.hpp"
#include "Hit.hpp"
#include "Ray.hpp"
#include "Plane.hpp"
#include "Material.h"

using namespace std;


struct Triangle : Object {
	glm::vec3 a, b, c, o, n_a, n_b, n_c;
	glm::vec3 min, max;

	Triangle() {};
	Triangle(const Triangle& other) = default;
	Triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 n_a, glm::vec3 n_b, glm::vec3 n_c) : a(a), b(b), c(c), n_a(n_a), n_b(n_b), n_c(n_c) {
		o = (a + b + c) / 3.0f;
		for (int i = 0; i < 3; i++) {
			min[i] = std::min(a[i], std::min(b[i], c[i]));
			max[i] = std::max(a[i], std::max(b[i], c[i]));
		}
	}
	Triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c) : Triangle(a, b, c, {0,0,-1}, {0,0,-1}, {0,0,-1}) {}

	Hit intersect(const Ray &ray) override {
		Hit hit;

		glm::vec3 local_o = inverseTransformationMatrix * glm::vec4(ray.origin, 1.0);
		glm::vec3 local_d = inverseTransformationMatrix * glm::vec4(ray.direction, 0.0);
		local_d = glm::normalize(local_d);

		glm::vec3 normal = glm::normalize(glm::cross(b - a, c - a));
		float DdotN = glm::dot(local_d, normal);
        if (DdotN > 0) return hit;

        // first test if there is intersection with the plane onto which the triangle lies.
		Plane plane = Plane(o, normal);
		Hit plane_hit = plane.intersect(Ray(local_o, local_d));
		if (!plane_hit.hit) return hit;

		glm::vec3 p = plane_hit.intersection;

		float area = glm::length(glm::cross(b - a, c - a));
        // if there was a hit with the place, check if the hit was inside the triangle
        // using the barycentric coordinates test.
		glm::vec3 vec_a = glm::cross(b - p, c - p);
		float area_a = glm::length(vec_a) * (glm::dot(vec_a, normal) < 0 ? -1.0f : 1.0f);
		glm::vec3 vec_b = glm::cross(c - p, a - p);
		float area_b = glm::length(vec_b) * (glm::dot(vec_b, normal) < 0 ? -1.0f : 1.0f);
		glm::vec3 vec_c = glm::cross(a - p, b - p);
		float area_c = glm::length(vec_c) * (glm::dot(vec_c, normal) < 0 ? -1.0f : 1.0f);

		if ((area_a < 0) || (area_b < 0) || (area_c < 0)) return hit;

        // Compute interpolated normal
		glm::vec3 n = glm::normalize(area_a * n_a + area_b * n_b + area_c * n_c);
//		n = normal;

		hit.hit = true;
		hit.intersection = transformationMatrix * glm::vec4(p, 1.0);
		hit.normal = glm::normalize(normalMatrix * glm::vec4(n, 0.0));
		hit.distance = glm::distance(ray.origin, hit.intersection);
		hit.object = this;
		return hit;
	}
};

#endif