#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include <vector>
#include <algorithm>
#include <sstream>
#include <cmath>

#include "glm/glm.hpp"
#include "Triangle.hpp"
#include "Ray.hpp"
#include "Hit.hpp"
#include "OBJ.hpp"

using namespace std;
using namespace OBJ;

struct BoundingBox {
	glm::vec3 min, max;
	vector<Triangle> triangles;
	int maxSize = 3;
	BoundingBox *left = nullptr, *right = nullptr;
	Model *model = nullptr;
	int level;

	BoundingBox();
	BoundingBox(vector<Triangle> T, int axis, Model *m, int l) {
		level = l;
		model = m;
		if (T.size() == 0) {
			throw "Empty triangle list";
			return;
		}
		
		if (T.size() <= maxSize) {
			min = 1000000000.0f * glm::vec3(1, 1, 1);
			max = -1000000000.0f * glm::vec3(1, 1, 1);
			for (int i = 0; i < T.size(); i++) {
				Triangle t = T[i];
				for (int d = 0; d < 3; d++) {
					min[d] = glm::min(min[d], t.min[d]);
					max[d] = glm::max(max[d], t.max[d]);
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
			left = new BoundingBox(vector<Triangle>(T.begin(), T.begin() + mid), (axis + 1) % 3, model, l + 1);
			right = new BoundingBox(vector<Triangle>(T.begin() + mid, T.end()), (axis + 1) % 3, model, l + 1);
			for (int d = 0; d < 3; d++) {
				min[d] = glm::min(left->min[d], right->min[d]);
				max[d] = glm::max(left->max[d], right->max[d]);
			}
		}
	}
	BoundingBox(vector<Triangle> &T) : BoundingBox(T, 0, nullptr, 0) {};
	BoundingBox(Model &M) : BoundingBox(M.triangles, 0, &M, 0) {};

	~BoundingBox() {
		if (left) delete left;
		if (right) delete right;
	}

	bool intersect(const Ray &ray) const {
		float tnear = -1000000000.0f, tfar = 1000000000.0f;
		for (int i = 0; i < 3; i++) {
			if (ray.direction[i] == 0) {
				if (ray.origin[i] < min[i] || ray.origin[i] > max[i]) {
					return false;
				}
			} else {
				float t1 = (min[i] - ray.origin[i]) / ray.direction[i];
				float t2 = (max[i] - ray.origin[i]) / ray.direction[i];
				if (t1 > t2) {
					float tmp = t1;
					t1 = t2;
					t2 = tmp;
				}
				tnear = t1 > tnear ? t1 : tnear;
				tfar = t2 < tfar ? t2 : tfar;
				if (tnear > tfar || tfar < 0) {
					return false;
				}
			}
		}
		return true;
	}

	vector<Hit> trace_ray(const Ray &ray) const {
		Ray R = ray;
		if (model) {
			glm::vec3 local_o = model->inverseTransformationMatrix * glm::vec4(ray.origin, 1.0);
			glm::vec3 local_d = model->inverseTransformationMatrix * glm::vec4(ray.direction, 0.0);
			local_d = glm::normalize(local_d);
			R = Ray(local_o, local_d);
		}
		vector<Hit> hits;
		// if (intersect(R)) {
		if (true) {
			if (left) {
				vector<Hit> leftHits = left->trace_ray(ray);
				for (Hit hit : leftHits) {
					hits.push_back(hit);
				}
			}
			if (right) {
				vector<Hit> rightHits = right->trace_ray(ray);
				for (Hit hit : rightHits) {
					hits.push_back(hit);
				}
			}
			if (triangles.size() > 0) {
				for (Triangle t : triangles) {
					Hit hit = t.intersect(R);
					if (hit.hit) {
						if (model) {
							hit.intersection = model->transformationMatrix * glm::vec4(hit.intersection, 1.0);
							hit.normal = glm::normalize(model->normalMatrix * glm::vec4(hit.normal, 0.0));
							hit.distance = glm::length(hit.intersection - ray.origin);
							// hit.debug = true;
						}
						hits.push_back(hit);
					}
				}
			}
		}
		return hits;
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