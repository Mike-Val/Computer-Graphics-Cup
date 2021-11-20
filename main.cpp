/**
@file main.cpp
*/

#include <iostream>
#include <fstream>
#include <cmath>
#include <ctime>
#include <vector>
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

#include "Utils.hpp"
#include "Image.h"
#include "Material.h"
#include "Triangle.hpp"
#include "Tree.hpp"
#include "OBJ.hpp"
#include "Ray.hpp"
#include "Object.hpp"
#include "Hit.hpp"
#include "Sphere.hpp"
#include "Plane.hpp"
#include "Cone.hpp"
#include "Light.hpp"

#include "Scene.hpp"

using namespace std;




int main(int argc, const char * argv[]) {
	OBJ::Model model = OBJ::read("models/bunny.obj");

	cout << model;

	return 0;




    clock_t t = clock(); // variable for keeping the time of the rendering

    int width = 1024; //width of the image
    int height = 768; // height of the image
    float fov = 90; // field of view

	sceneDefinition(); // Let's define a scene

	Image image(width,height); // Create an image where we will store the result

    float s = 2*tan(0.5*fov/180*M_PI)/width;
    float X = -s * width / 2;
    float Y = s * height / 2;

    for(int i = 0; i < width ; i++)
        for(int j = 0; j < height ; j++){

			float dx = X + i*s + s/2;
            float dy = Y - j*s - s/2;
            float dz = 1;

			glm::vec3 origin(0, 0, 0);
            glm::vec3 direction(dx, dy, dz);
            direction = glm::normalize(direction);

            Ray ray(origin, direction);

			image.setPixel(i, j, toneMapping(trace_ray(lights, ambient_light, objects, ray)));

        }

    t = clock() - t;
    cout<<"It took " << ((float)t)/CLOCKS_PER_SEC<< " seconds to render the image."<< endl;
    cout<<"I could render at "<< (float)CLOCKS_PER_SEC/((float)t) << " frames per second."<<endl;

	// Writing the final results of the rendering
	if (argc == 2){
		image.writeImage(argv[2]);
	}else{
		image.writeImage("./result.ppm");
	}

    return 0;
}
