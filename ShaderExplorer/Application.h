#pragma once
#include <SDL2/SDL.h>
#undef main
#include <string>

#include "DebugHelper.h"
#include "Renderer.h"


class Application
{
public:
	Application();
	~Application();

	bool InitWindow(std::string caption, int width = 1280, int height = 720);
	void Run();
	

private:
	SDL_Window * window_;
	Renderer renderer_;
};

