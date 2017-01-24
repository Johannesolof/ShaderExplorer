#pragma once
#include "Camera.h"
#include <vector>
#include "Light.h"
#include "Object.h"
#include <memory>

class Scene
{
public:
	Scene(std::shared_ptr<Camera> camera);
	~Scene();

	void Update();

	std::shared_ptr<Camera> main_camera;
	std::vector<std::shared_ptr<Light>> lights;
	std::vector<std::shared_ptr<Object>> objects;
};

