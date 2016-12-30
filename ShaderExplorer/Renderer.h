#pragma once

#include <SDL2/SDL.h>
#include "glm/glm.hpp"

#include "Shader.h"

class Renderer
{
public:
	Renderer();
	~Renderer();

	void InitGl();
	void Display(SDL_Window *window);

private:
	Shader simple_shader_;
	GLuint vertex_array_object_;
	glm::vec3 clear_color_;
};

