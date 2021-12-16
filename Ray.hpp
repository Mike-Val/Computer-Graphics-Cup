#ifndef RAY_HPP
#define RAY_HPP

#include <limits>

#include "glm/glm.hpp"

#define FLOAT_INFINITY std::numeric_limits<float>::infinity()

/**
 Class representing a single ray.
 */
class Ray{
public:
    glm::vec3 origin; ///< Origin of the ray
    glm::vec3 direction; ///< Direction of the ray
	glm::vec3 inv_direction; ///< Inverse of the direction of the ray
	bool sign[3]; ///< Sign of the direction of the ray
	/**
	 Contructor of the ray
	 @param origin Origin of the ray
	 @param direction Direction of the ray
	 */
    Ray(glm::vec3 origin, glm::vec3 direction) : origin(origin), direction(direction) {
		inv_direction = glm::vec3(direction.x == 0 ? FLOAT_INFINITY : 1.0f / direction.x, 
									direction.y == 0 ? FLOAT_INFINITY : 1.0f / direction.y, 
									direction.z == 0 ? FLOAT_INFINITY : 1.0f / direction.z);
		sign[0] = (inv_direction.x < 0);
		sign[1] = (inv_direction.y < 0);
		sign[2] = (inv_direction.z < 0);
    }

	Ray(const Ray& other) = default;
};

#endif