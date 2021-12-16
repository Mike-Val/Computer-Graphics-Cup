#ifndef HIT_HPP
#define HIT_HPP

#include <limits>

#include "glm/glm.hpp"
#include "Object.hpp"

#define FLOAT_INFINITY std::numeric_limits<float>::infinity()


class Object;

/**
 Structure representing the even of hitting an object
 */
struct Hit{
    bool hit; ///< Boolean indicating whether there was or there was no intersection with an object
    glm::vec3 normal; ///< Normal vector of the intersected object at the intersection point
    glm::vec3 intersection; ///< Point of Intersection
    float distance; ///< Distance from the origin of the ray to the intersection point
    Object *object; ///< A pointer to the intersected object
	glm::vec2 uv; ///< Coordinates for computing the texture (texture coordinates)

    bool debug = false;

    Hit() : hit(false), distance(FLOAT_INFINITY), object(nullptr) {}
    Hit(const Hit &h) : hit(h.hit), normal(h.normal), intersection(h.intersection), distance(h.distance), object(h.object), uv(h.uv) {}
};

#endif