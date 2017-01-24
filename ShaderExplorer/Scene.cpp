#include "Scene.h"

Scene::Scene(std::shared_ptr<Camera> camera)
{
	main_camera = camera;
}

Scene::~Scene()
{
}

void Scene::Update()
{
	main_camera->Update();
	for (auto&& light : objects)
	{
		light->Update();
	}
	for (auto&& object : objects)
	{
		object->Update();
	}
}
