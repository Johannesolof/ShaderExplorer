#pragma once

#include <vector>
#include <SDL2/SDL.h>
#include "glm/glm.hpp"

#include "Shader.h"
#include "Model.h"
#include "Scene.h"

class Renderer
{
public:
	Renderer();
	~Renderer();

	void InitGl();
	void Display(SDL_Window* window, std::shared_ptr<Scene> scene);

private:

	Shader simple_shader_;
	glm::vec3 clear_color_;

	glm::vec3 cameraPosition = glm::vec3(-70.0f, 50.0f, 70.0f);
	glm::vec3 cameraDirection = glm::normalize(glm::vec3(0.0f) - glm::vec3(-70.0f, 50.0f, 70.0f));
	Model * testModel = new Model();
};

