#ifndef UTIL_HPP
#define UTIL_HPP

#include <vector>

#include "glm/glm.hpp"
#include "Object.hpp"
#include "Light.hpp"
#include "Hit.hpp"
#include "BoundingBox.hpp"

using namespace std;

/**
 Function performing tonemapping of the intensities computed using the raytracer
 @param intensity Input intensity
 @return Tonemapped intensity in range (0,1)
 */
glm::vec3 toneMapping(const glm::vec3 &intensity) {
	float gamma = 1.0/2.0;
	float alpha = 12.0f;
	return glm::clamp(alpha * glm::pow(intensity, glm::vec3(gamma)), glm::vec3(0.0), glm::vec3(1.0));
}

float fresnel_factor(const glm::vec3 &reflect_dir, const glm::vec3 &refract_dir, const glm::vec3 &normal, const float &d1, const float &d2) {
	float cos1, cos2;
	cos1 = glm::dot(normal, reflect_dir);
	cos2 = glm::dot(-normal, refract_dir);

	float a = pow((d1*cos1 - d2*cos2) / (d1*cos1 + d2*cos2), 2);
	float b = pow((d1*cos2 - d2*cos1) / (d1*cos2 + d2*cos1), 2);

	float F = 0.5f * (a + b);

	return F;
}

/** Function for computing color of an object according to the Phong Model
 @param point A point belonging to the object for which the color is computed
 @param normal A normal vector the the point
 @param uv Texture coordinates
 @param view_direction A normalized direction from the point to the viewer/camera
 @param material A material structure representing the material of the object
*/
glm::vec3 PhongModel(const vector<Light *> &lights, 
					const glm::vec3 &ambient_light, 
					const vector<Object *> &objects, 
					const glm::vec3 &point, 
					const glm::vec3 &normal, 
					const glm::vec2 &uv, 
					const glm::vec3 &view_direction, 
					const Material &material,
                    const BoundingBox &bbox){

	glm::vec3 color(0.0);
	for(auto light : lights){

		glm::vec3 light_direction = glm::normalize(light->position - point);
		glm::vec3 reflected_direction = glm::reflect(-light_direction, normal);

		float NdotL = glm::clamp(glm::dot(normal, light_direction), 0.0f, 1.0f);
		float VdotR = glm::clamp(glm::dot(view_direction, reflected_direction), 0.0f, 1.0f);

		
		glm::vec3 diffuse_color = material.diffuse;
		 if(material.texture){
		 	diffuse_color = material.texture(uv);
		 }
		
		glm::vec3 diffuse = diffuse_color * glm::vec3(NdotL);
		glm::vec3 specular = material.specular * glm::vec3(pow(VdotR, material.shininess));
		
		
		// distance to the light
		float r = glm::distance(point,light->position);
		r = max(r, 0.1f);
		
		// Checking if the light source can be reached directly from the point
		Ray shadow_ray(point + light_direction * 0.01f, light_direction);
		Hit shadow_hit;
		for (auto o : objects) {
			shadow_hit = o->intersect(shadow_ray);
			if(shadow_hit.hit && shadow_hit.distance < r) break;
		}
        Hit bb_hit = bbox.trace_ray(shadow_ray);
        if (bb_hit.hit && bb_hit.distance < r) shadow_hit = bb_hit;
		if (!shadow_hit.hit || shadow_hit.distance > r)
			color += light->color * (diffuse + specular) / r/r;
	}
	color += ambient_light * material.ambient;
	
	color = glm::clamp(color, glm::vec3(0.0), glm::vec3(1.0));
	return color;
}

glm::vec3 trace_ray(const vector<Light *> &lights, 
					const glm::vec3 &ambient_light, 
					const vector<Object *> &objects,
					const Ray &ray, 
					const int &maxDepth,
					const BoundingBox &bbox) {
	Hit hit;

	hit.hit = false;
	hit.distance = INFINITY;

	for(auto object : objects){
		Hit hit_ = object->intersect(ray);
		if(hit_.hit && hit_.distance < hit.distance) hit = hit_;
	}
	Hit bb_hit = bbox.trace_ray(ray);
	if (bb_hit.hit && bb_hit.distance < hit.distance) hit = bb_hit;

	// if (hit.debug) return glm::vec3(1.0, 0.0, 0.0);
	// if (hit.debug) return glm::vec3(1.0 - (hit.distance - 6.0) / 10.0, 0.0, 0.0);
	// if (hit.debug) return (hit.normal+1.0f)/2.0f;

	if (!hit.hit) return {0,0,0};

	Material m = hit.object->getMaterial();
	float refraction_index = m.refraction;
	bool inside = glm::dot(hit.normal, -ray.direction) < 0;
	if (inside) hit.normal = -hit.normal;
	
	glm::vec3 phong = PhongModel(lights, ambient_light, objects, hit.intersection, hit.normal, hit.uv, glm::normalize(-ray.direction), m, bbox);
	if (maxDepth < 0) return phong;
	
	glm::vec3 reflect(0);
	glm::vec3 reflect_direction = glm::reflect(ray.direction, hit.normal);
	if (m.reflection > 0) {
		reflect = trace_ray(lights, ambient_light, objects, Ray(hit.intersection + reflect_direction * 0.001f, reflect_direction), maxDepth - 1, bbox) * m.reflection;
		return reflect + phong * (1 - m.reflection);
	}

	glm::vec3 refract(0);
	if (m.refraction > 0) {
		float d1, d2;
		if (inside) {
			d1 = refraction_index;
			d2 = 1.0f;
		} else {
			d1 = 1.0f;
			d2 = refraction_index;
		}
		glm::vec3 refract_direction = glm::refract(ray.direction, hit.normal, d1 / d2);
		float F = fresnel_factor(reflect_direction, refract_direction, hit.normal, d1, d2);

		glm::vec3 fresnel_reflect = F * trace_ray(lights, ambient_light, objects, Ray(hit.intersection + reflect_direction * 0.001f, reflect_direction), maxDepth - 1, bbox);
		glm::vec3 fresnel_refract = (1 - F) * trace_ray(lights, ambient_light, objects, Ray(hit.intersection + refract_direction * 0.001f, refract_direction), maxDepth - 1, bbox);

		refract = fresnel_reflect + fresnel_refract;
		return refract;
	}

	return phong;
}
/**
 Functions that computes a color along the ray
 @param ray Ray that should be traced through the scene
 @return Color at the intersection point
 */
glm::vec3 trace_ray(const vector<Light *> &lights, 
					const glm::vec3 &ambient_light, 
					const vector<Object *> &objects,
					const Ray &ray,
					const BoundingBox &bbox) {
	return trace_ray(lights, ambient_light, objects, ray, 5, bbox);
}


#endif