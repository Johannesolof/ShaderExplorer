#pragma once
#include <SDL2/SDL.h>

#include "Renderer.h"


class Application
{
public:
	Application(std::string caption, int width = 1280, int height = 720);
	~Application();

	bool InitWindow();
	void Run();
	

private:
	std::string caption_;
	int width_;
	int height_;
	SDL_Window * window_;
	Renderer renderer_;
};

