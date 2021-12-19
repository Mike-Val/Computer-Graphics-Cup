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

#include "thread_pool.hpp"
#include "Utils.hpp"
#include "Image.h"
#include "Material.h"
#include "Triangle.hpp"
#include "OBJ.hpp"
#include "Ray.hpp"
#include "Object.hpp"
#include "Hit.hpp"
#include "Sphere.hpp"
#include "Plane.hpp"
#include "Cone.hpp"
#include "Light.hpp"
#include "BoundingBox.hpp"

#include "Scene.hpp"

using namespace std;




int main(int argc, const char * argv[]) {
    Material model_material;
	model_material.ambient = glm::vec3(0.09f, 0.09f, 0.09f);
	model_material.diffuse = glm::vec3(0.6f, 0.6f, 0.6f);
    model_material.shininess = 100.0;
//    model_material.reflection = 1.0;
    model_material.refraction = 2.0;

    glm::mat4 translationMatrix = glm::translate(glm::vec3(3,4,15));
    float scale = 8;
	glm::mat4 scalingMatrix = glm::scale(glm::vec3(-scale, scale, -scale));
//	 glm::mat4 rotationMatrix = glm::rotate(glm::radians(55.0f) , glm::vec3(0,1,0)) * glm::rotate(glm::radians(15.0f) , glm::vec3(0,0,1));
	glm::mat4 rotationMatrix = glm::rotate(glm::radians(55.0f) , glm::vec3(0,1,0));
	glm::mat4 modelMatrix = translationMatrix * rotationMatrix * scalingMatrix;

	OBJ::Model model = OBJ::read("../models/skull.obj");
    model.material = model_material;
    model.setTransformation(modelMatrix);

	cout << model << endl;

    BoundingBox bbox = BoundingBox(model);

    int width = 1024*4; //width of the image
    int height = 768*4; // height of the image
    float fov = 90; // field of view

	sceneDefinition(); // Let's define a scene


	Image image(width,height); // Create an image where we will store the result

    float s = 2*tan(0.5*fov/180*M_PI)/width;
    float X = -s * width / 2;
    float Y = s * height / 2;

    thread_pool pool;

    int threads = pool.get_thread_count();
    cout << "Threads: " << threads << endl;

    clock_t t = clock(); // variable for keeping the time of the rendering

    auto task = [&image, &X, &Y, &s, &width, &height, &bbox](int y_min, int y_max){
                    for(int j = y_min; j < min(y_max, height) ; j++) {
                        for(int i = 0; i < width ; i++) {
                            float dx = X + i*s + s/2;
                            float dy = Y - j*s - s/2;
                            float dz = 1;
                            glm::vec3 origin(0, 0, 0);
                            glm::vec3 direction(dx, dy, dz);
                            direction = glm::normalize(direction);
                            Ray ray(origin, direction);
                            try {
                                image.setPixel(i, j, toneMapping(trace_ray(lights, ambient_light, objects, ray, bbox)));
                            } catch (...) {
                                image.setPixel(i, j, glm::vec3(0,0,0));
                                cout << "Error at pixel: " << i << " " << j << endl;
                            }
                        }
                    }
                };

     for (int a = 0; a < threads; a++) {
         pool.push_task(task,
                         int(a*(double(height)/threads)),
                         int((a+1)*(double(height)/threads)));
     }
     cout << "Submitted tasks" << endl;
     pool.wait_for_tasks();
     cout << "Finished tasks" << endl;

    t = clock() - t;
    cout<<"It took " << ((float)t)/CLOCKS_PER_SEC<< " seconds to render the image."<< endl;
    cout<<"I could render at "<< (float)CLOCKS_PER_SEC/((float)t) << " frames per second."<<endl;

	// Writing the final results of the rendering
	if (argc == 2){
		image.writeImage(argv[2]);
	}else{
		image.writeImage("../result.ppm");
	}

    return 0;
}
