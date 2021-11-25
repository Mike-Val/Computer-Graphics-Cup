#ifndef OBJ_HPP
#define OBJ_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <sstream>

#include "glm/glm.hpp"
#include "Triangle.hpp"
#include "Material.h"

using namespace std;

namespace OBJ {

struct Model {
	bool good;
	string name;
	vector<Triangle> triangles;
	Material material;

	glm::mat4 transformationMatrix;
	glm::mat4 inverseTransformationMatrix;
	glm::mat4 normalMatrix;


	Model(string name) : Model(name, true) {}
	Model(string name, bool good) : name(name), good(good) {}

	void addTriangle(Triangle t) {
		triangles.push_back(t);
	}

	string toString() const {
		stringstream ss;
		ss << "OBJ::Model (" << name << "): " << triangles.size() << " triangles";
		return ss.str();
	}
	
	friend ostream& operator<<(ostream& os, const Model& obj) {
		os << obj.toString();
		return os;
	}

	void setTransformation(glm::mat4 matrix){
		transformationMatrix = matrix;
		inverseTransformationMatrix = glm::inverse(matrix);
		normalMatrix = glm::transpose(inverseTransformationMatrix);
	}
};

// Assuming no texture coordinates are present in the OBJ file
Model read(const string &filename) {
	Model model("", false);
	// Open file
	ifstream file(filename);
	if (!file.is_open()) {
		cerr << "Could not open file " << filename << endl;
		return model;
	}
	string name;
	vector<glm::vec3> vertices;
	vector<glm::vec3> normals;
	string line;
	while (getline(file, line)) {
		if (line.substr(0, 1) == "o") {
			model.name = line.substr(2, line.length() - 2);
		} else if (line.substr(0,2) == "vn") {
			stringstream ss(line.substr(3));
			glm::vec3 normal;
			ss >> normal.x >> normal.y >> normal.z;
			normals.push_back(normal);
		} else if (line.substr(0,1) == "v") {
			stringstream ss(line.substr(2));
			glm::vec3 vertex;
			ss >> vertex.x >> vertex.y >> vertex.z;
			vertices.push_back(vertex);
		} else if (line.substr(0,1) == "f") {
			stringstream ss(line.substr(2));
			vector<int> face_vertices, face_normals;
			string token;
			while (getline(ss, token, ' ')) {
				stringstream token_ss(token);
				int vertex = 0, normal;
				string index;
				while (getline(token_ss, index, '/')) {
					if (index.length() > 0) {
						stringstream index_ss(index);
						if (vertex == 0) {
							index_ss >> vertex;
						}
						else index_ss >> normal;
					}
				}
				face_vertices.push_back(vertex - 1);
				face_normals.push_back(normal - 1);
			}
			model.addTriangle(Triangle(vertices[face_vertices[0]],
										vertices[face_vertices[1]],
										vertices[face_vertices[2]]));
		}
	}
	model.good = true;
	return model;
}

} // namespace OBJReader

#endif