#include <vector>

#include "glm/glm.hpp"
#include "Utils.hpp"
#include "Object.hpp"
#include "Light.hpp"
#include "Sphere.hpp"
#include "Plane.hpp"
#include "Cone.hpp"

#ifndef SCENE_HPP
#define SCENE_HPP

using namespace std;

vector<Light *> lights; ///< A list of lights in the scene
glm::vec3 ambient_light(0.001,0.001,0.001);
vector<Object *> objects; ///< A list of all objects in the scene

/**
 Function defining the scene
 */
void sceneDefinition (){

	Material green_diffuse;
	green_diffuse.ambient = glm::vec3(0.03f, 0.1f, 0.03f);
	green_diffuse.diffuse = glm::vec3(0.3f, 1.0f, 0.3f);

	Material red_specular;
	red_specular.diffuse = glm::vec3(1.0f, 0.2f, 0.2f);
	red_specular.ambient = glm::vec3(0.01f, 0.02f, 0.02f);
	red_specular.specular = glm::vec3(0.5);
	red_specular.shininess = 10.0;

	Material blue_specular;
	blue_specular.ambient = glm::vec3(0.02f, 0.02f, 0.1f);
	blue_specular.diffuse = glm::vec3(0.2f, 0.2f, 1.0f);
	blue_specular.specular = glm::vec3(0.6);
	blue_specular.shininess = 100.0;
	blue_specular.reflection = 1.0f;


	// objects.push_back(new Sphere(1.0, glm::vec3(1,-2,8), blue_specular));
	// objects.push_back(new Sphere(0.5, glm::vec3(-1,-2.5,6), red_specular));
	//objects.push_back(new Sphere(1.0, glm::vec3(3,-2,6), green_diffuse));
	
	
	//Textured sphere
	
	Material textured;
	textured.texture = &rainbowTexture;
	// objects.push_back(new Sphere(7.0, glm::vec3(-6,4,23), textured));


	Material glass;
	glass.refraction = 2.0f;

	// objects.push_back(new Sphere(2.0, glm::vec3(-3, -1, 8), glass));
	
	
	//Planes
	Material red_diffuse;
	red_diffuse.ambient = glm::vec3(0.09f, 0.06f, 0.06f);
	red_diffuse.diffuse = glm::vec3(0.9f, 0.6f, 0.6f);
		
	Material blue_diffuse;
	blue_diffuse.ambient = glm::vec3(0.06f, 0.06f, 0.09f);
	blue_diffuse.diffuse = glm::vec3(0.6f, 0.6f, 0.9f);
	// objects.push_back(new Plane(glm::vec3(0,-3,0), glm::vec3(0.0,1,0)));
	objects.push_back(new Plane(glm::vec3(0,1,30), glm::vec3(0.0,0.0,-1.0), green_diffuse));
	// objects.push_back(new Plane(glm::vec3(-15,1,0), glm::vec3(1.0,0.0,0.0), red_diffuse));
	// objects.push_back(new Plane(glm::vec3(15,1,0), glm::vec3(-1.0,0.0,0.0), blue_diffuse));
	// objects.push_back(new Plane(glm::vec3(0,27,0), glm::vec3(0.0,-1,0)));
	// objects.push_back(new Plane(glm::vec3(0,1,-0.01), glm::vec3(0.0,0.0,1.0), green_diffuse));
	
	
	// Cone
	
	Material yellow_specular;
	yellow_specular.ambient = glm::vec3(0.1f, 0.10f, 0.0f);
	yellow_specular.diffuse = glm::vec3(0.4f, 0.4f, 0.0f);
	yellow_specular.specular = glm::vec3(1.0);
	yellow_specular.shininess = 100.0;
	
	Cone *cone = new Cone(yellow_specular);
	glm::mat4 translationMatrix = glm::translate(glm::vec3(5,9,14));
	glm::mat4 scalingMatrix = glm::scale(glm::vec3(3.0f, 12.0f, 3.0f));
	glm::mat4 rotationMatrix = glm::rotate(glm::radians(180.0f) , glm::vec3(1,0,0));
	cone->setTransformation(translationMatrix*scalingMatrix*rotationMatrix);
	// objects.push_back(cone);
	
	Cone *cone2 = new Cone(green_diffuse);
	translationMatrix = glm::translate(glm::vec3(6,-3,7));
	scalingMatrix = glm::scale(glm::vec3(1.0f, 3.0f, 1.0f));
	rotationMatrix = glm::rotate(glm::atan(3.0f), glm::vec3(0,0,1));
	cone2->setTransformation(translationMatrix* rotationMatrix*scalingMatrix);
	// objects.push_back(cone2);
	
	
	lights.push_back(new Light(glm::vec3(0, 26, 5), glm::vec3(1.0, 1.0, 1.0)));
	lights.push_back(new Light(glm::vec3(0, 1, 12), glm::vec3(0.1)));
	lights.push_back(new Light(glm::vec3(0, 5, 1), glm::vec3(0.4)));
}

#endif