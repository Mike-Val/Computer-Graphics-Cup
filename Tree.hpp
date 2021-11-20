#include <vector>

#include "glm/glm.hpp"
#include "Triangle.hpp"

#ifndef TREE_HPP
#define TREE_HPP

using namespace std;

struct Tree {
	int level;
	vector<Triangle> triangles;
	struct Tree *left, *right;

	Tree(Tree &other) = default;
	Tree(Triangle triangle) : level(0), left(nullptr), right(nullptr) {
		triangles.push_back(triangle);
	}
	~Tree() {
		if (left) delete left;
		if (right) delete right;
	}

	void insert(const Triangle t) {
		int dimension = level % 3;
		if (t.o[dimension] < triangles[0].o[dimension]) {
			if (left) left->insert(t);
			else left = new Tree(t);
		} else if (t.o[dimension] > triangles[0].o[dimension]) {
			if (right) right->insert(t);
			else right = new Tree(t);
		} else {
			triangles.push_back(t);
		}
	}
};

#endif