#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include <vector>
#include <algorithm>
#include <sstream>
#include <cmath>
#include <limits>

#include "glm/glm.hpp"
#include "Triangle.hpp"
#include "Ray.hpp"
#include "Hit.hpp"
#include "OBJ.hpp"

#define FLOAT_INFINITY std::numeric_limits<float>::infinity()

using namespace std;
using namespace OBJ;

struct BoundingBox {
	glm::vec3 min, max;
	vector<Triangle> triangles;
	int maxSize = 1;
	BoundingBox *left = nullptr, *right = nullptr;
	Model *model = nullptr;
	int level;

	BoundingBox();
	BoundingBox(vector<Triangle> &T, int axis, Model *m, int l) {
		level = l;
		model = m;
		if (T.size() == 0) {
			cout << "Empty Bounding Box" << endl;
			throw "Empty triangle vector";
			return;
		}
		
		if (T.size() <= maxSize) {
			min = FLOAT_INFINITY * glm::vec3(1, 1, 1);
			max = -FLOAT_INFINITY * glm::vec3(1, 1, 1);
			for (int i = 0; i < T.size(); i++) {
				Triangle t = T[i];
				for (int d = 0; d < 3; d++) {
					min[d] = std::min(min[d], t.min[d]);
					max[d] = std::max(max[d], t.max[d]);
				}
				if (model) t.setTransformation(glm::mat4(1.0f));
				triangles.push_back(t);
			}
			return;
		} else {
			sort(T.begin(), T.end(), [axis](Triangle a, Triangle b) {
				return a.o[axis] < b.o[axis];
			});

			int mid = T.size() / 2;
			vector<Triangle> leftT(T.begin(), T.begin() + mid);
			vector<Triangle> rightT(T.begin() + mid, T.end());

			left = new BoundingBox(leftT, (axis + 1) % 3, model, l + 1);
			right = new BoundingBox(rightT, (axis + 1) % 3, model, l + 1);
			for (int d = 0; d < 3; d++) {
				min[d] = std::min(left->min[d], right->min[d]);
				max[d] = std::max(left->max[d], right->max[d]);
			}
		}
	}
	BoundingBox(vector<Triangle> &T) : BoundingBox(T, 0, nullptr, 0) {};
	BoundingBox(Model &M) : BoundingBox(M.triangles, 0, &M, 0) {};

	~BoundingBox() {
		if (left) delete left;
		if (right) delete right;
	}

	bool intersect(const Ray &ray, float t0=0, float t1=FLOAT_INFINITY) const {
		// return true;
		float tmin, tmax, tymin, tymax, tzmin, tzmax;
		glm::vec3 bounds[] = {min, max};

		tmin = (bounds[ray.sign[0]].x - ray.origin.x) * ray.inv_direction.x;
		tmax = (bounds[1-ray.sign[0]].x - ray.origin.x) * ray.inv_direction.x;
		tymin = (bounds[ray.sign[1]].y - ray.origin.y) * ray.inv_direction.y;
		tymax = (bounds[1-ray.sign[1]].y - ray.origin.y) * ray.inv_direction.y;

		if ((tmin > tymax) || (tymin > tmax)) return false;
		if (tymin > tmin) tmin = tymin;
		if (tymax < tmax) tmax = tymax;

		tzmin = (bounds[ray.sign[2]].z - ray.origin.z) * ray.inv_direction.z;
		tzmax = (bounds[1-ray.sign[2]].z - ray.origin.z) * ray.inv_direction.z;

		if ((tmin > tzmax) || (tzmin > tmax)) return false;
		if (tzmin > tmin) tmin = tzmin;
		if (tzmax < tmax) tmax = tzmax;

		return ((tmin < t1) && (tmax > t0));
	}

	Hit trace_ray(const Ray &ray) const {
		Ray R = ray;
		if (model) {
			glm::vec3 local_o = model->inverseTransformationMatrix * glm::vec4(ray.origin, 1.0);
			glm::vec3 local_d = model->inverseTransformationMatrix * glm::vec4(ray.direction, 0.0);
			local_d = glm::normalize(local_d);
			R = Ray(local_o, local_d);
		}
		Hit bestHit, tmpHit;
		if (intersect(R)) {
			if (left) tmpHit = left->trace_ray(ray);
			if (tmpHit.distance < bestHit.distance) bestHit = tmpHit;
			if (right) tmpHit = right->trace_ray(ray);
			if (tmpHit.distance < bestHit.distance) bestHit = tmpHit;
			if (triangles.size() > 0) {
				for (Triangle t : triangles) {
					tmpHit = t.intersect(R);
					if (tmpHit.hit) {
						if (model) {
							tmpHit.intersection = model->transformationMatrix * glm::vec4(tmpHit.intersection, 1.0);
							tmpHit.normal = glm::normalize(model->normalMatrix * glm::vec4(tmpHit.normal, 0.0));
							tmpHit.distance = glm::length(tmpHit.intersection - ray.origin);
							tmpHit.debug = true;
						}
						if (tmpHit.distance < bestHit.distance) bestHit = tmpHit;
					}
				}
			}
		}
		return bestHit;
	}

	int boxes() const {
		return 1 + (left ? left->boxes() : 0) + (right ? right->boxes() : 0);
	}
	int leaves() const {
		return (left ? left->leaves() : 0) + (right ? right->leaves() : 0) + (triangles.size() > 0 ? 1 : 0);
	}
	int count() const {
		return triangles.size() + (left ? left->count() : 0) + (right ? right->count() : 0);
	}
	int depth() const {
		return 1 + std::max((left ? left->depth() : 0), (right ? right->depth() : 0));
	}

};

#endif