#include "Renderer.h"
#include <GL/glew.h>
#include <fstream>
#include "DebugHelper.h"
#include "Shader.h"
#include <SDL2/SDL.h>


Renderer::Renderer()
{
	simple_shader_ = Shader();
	vertex_array_object_ = GLuint(0);
	clear_color_ = glm::vec3(0.1f, 0.1f, 0.1f);
}


Renderer::~Renderer()
{
}

void Renderer::InitGl()
{
	//////////////////////////////////////////////////////////////////////////////
	// Vertex positions
	//////////////////////////////////////////////////////////////////////////////
	// Define the positions for each of the three vertices of the triangle
	const float positions[] = {
		//	 X      Y     Z
		0.0f,   0.5f, 1.0f,		// v0
		-0.5f,  -0.5f, 1.0f,	// v1
		0.5f,  -0.5f, 1.0f		// v2
	};
	// Create a handle for the position vertex buffer object
	GLuint positionBuffer;
	glGenBuffers(1, &positionBuffer);
	// Set the newly created buffer as the current one
	glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
	// Send the vertex position data to the current buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);

	//////////////////////////////////////////////////////////////////////////////
	// Vertex colors
	//
	// TASK 4: Change these colors to something more fun. 
	//////////////////////////////////////////////////////////////////////////////
	// Define the colors for each of the three vertices of the triangle
	const GLfloat colors[] = {
		//  R     G		B
		0.4f, 0.5f, 0.8f,		// White
		0.5f, 0.1f, 0.5f,		// White
		0.2f, 0.7f, 0.5f		// White
	};
	// Create a handle for the vertex color buffer
	GLuint colorBuffer;
	glGenBuffers(1, &colorBuffer);
	// Set the newly created buffer as the current one
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	// Send the vertex color data to the current buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

	glGenVertexArrays(1, &vertex_array_object_);
	// Bind the vertex array object
	// The following calls will affect this vertex array object.
	glBindVertexArray(vertex_array_object_);
	// Makes positionBuffer the current array buffer for subsequent calls.
	glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
	// Attaches positionBuffer to vertex_array_object_, in the 0th attribute location
	glVertexAttribPointer(0, 3, GL_FLOAT, false/*normalized*/, 0/*stride*/, 0/*offset*/);
	// Makes colorBuffer the current array buffer for subsequent calls.
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	// Attaches colorBuffer to vertex_array_object_, in the 1st attribute location
	glVertexAttribPointer(1, 3, GL_FLOAT, false/*normalized*/, 0/*stride*/, 0/*offset*/);
	glEnableVertexAttribArray(0); // Enable the vertex position attribute
	glEnableVertexAttribArray(1); // Enable the vertex color attribute 

	simple_shader_ = Shader();
	simple_shader_.LoadShader("simple.vert", "simple.frag", false);

}

void Renderer::Display(SDL_Window *window)
{
	// The viewport determines how many pixels we are rasterizing to
	int w, h;
	SDL_GetWindowSize(window, &w, &h);
	glViewport(0, 0, w, h);		// Set viewport

								///////////////////////////////////////////////////////////////////////////
								// TASK 1: Create a bug here to try the debugger.
								///////////////////////////////////////////////////////////////////////////
	glClearColor(clear_color_.r, clear_color_.g, clear_color_.b, 1.0);	// Set clear color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clears the color buffer and the z-buffer

														// We disable backface culling for this tutorial, otherwise care must be taken with the winding order
														// of the vertices. It is however a lot faster to enable culling when drawing large scenes.
	glDisable(GL_CULL_FACE);

	// Shader Program
	glUseProgram(simple_shader_.program);			// Set the shader program to use for this draw call
											// Bind the vertex array object that contains all the vertex data.
	glBindVertexArray(vertex_array_object_);
	// Submit triangles from currently bound vertex array object.
	glDrawArrays(GL_TRIANGLES, 0, 3);				// Render 1 triangle

	glBindVertexArray(vertex_array_object_);
	// Submit triangles from currently bound vertex array object.
	glDrawArrays(GL_TRIANGLES, 0, 3);				// Render 1 triangle


	glUseProgram(0);						// "unsets" the current shader program. Not really necessary.
}
