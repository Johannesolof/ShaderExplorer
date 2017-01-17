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
	void Display(SDL_Window* window, Scene *scene);

private:
	Shader simple_shader_;
	glm::vec3 clear_color_;
};

