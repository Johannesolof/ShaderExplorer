#pragma once
#include "Camera.h"
#include <vector>
#include "Light.h"
#include "Object.h"

class Scene
{
public:
	Scene() : main_camera(glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)) { }
	~Scene();

	Camera main_camera;
	std::vector<Light> lights;
	std::vector<Object*> objects;
};

