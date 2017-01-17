#include "Renderer.h"

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <glm/gtx/transform.hpp>

#include "Shader.h"
#include "Camera.h"
#include "DebugHelper.h"
#include "Model.h"

Renderer::Renderer()
{
	simple_shader_ = Shader();
	clear_color_ = glm::vec3(0.1f, 0.1f, 0.1f);
}


Renderer::~Renderer()
{
}

void Renderer::InitGl()
{
	simple_shader_.LoadShader("../Assets/Shaders/simple.vert", "../Assets/Shaders/simple.frag", false);


}

void Renderer::Display(SDL_Window *window, Scene *scene)
{
	int w, h;
	SDL_GetWindowSize(window, &w, &h);
	///////////////////////////////////////////////////////////////////////////
	// setup matrices
	///////////////////////////////////////////////////////////////////////////
	glm::mat4 projMatrix = glm::perspective(
		glm::radians(45.0f), 
		float(w) / float(h), 
		5.0f, 500.0f);
	
	glm::mat4 viewMatrix = glm::lookAt(
		scene->main_camera.transform.Position(),
		scene->main_camera.transform.Position() + scene->main_camera.transform.Forward(),
		scene->main_camera.transform.Up());

	glm::vec3 lightPosition = glm::vec3(40.0f, 40.0f, 0.0f);
	glm::mat4 lightViewMatrix = lookAt(lightPosition, glm::vec3(0.0f), Transform::WorldUp);
	glm::mat4 lightProjMatrix = glm::perspective(glm::radians(45.0f), 1.0f, 25.0f, 100.0f);

	///////////////////////////////////////////////////////////////////////////
	// Draw from camera
	///////////////////////////////////////////////////////////////////////////
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, w, h);
	glClearColor(clear_color_.r, clear_color_.g, clear_color_.b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (Object * object : scene->objects) {
		simple_shader_.UseProgram();
		simple_shader_.SetUniformSlow("modelViewProjectionMatrix", projMatrix * viewMatrix * object->transform.Matrix());
		object->model->Render();
	}

	CHECK_GL_ERROR();

	glUseProgram(0);						// "unsets" the current shader program_. Not really necessary.
}

