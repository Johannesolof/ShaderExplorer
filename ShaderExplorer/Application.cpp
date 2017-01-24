#include "Application.h"

#include <GL/glew.h>
#include <vector>

#include "DebugHelper.h"

//#define HDR_FRAMEBUFFER

Application::Application(std::string caption, int width, int height)
{
	caption_ = caption;
	width_ = width;
	height_ = height;

	window_ = nullptr;
	renderer_ = Renderer();
}

Application::~Application()
{
	//Destroy window
	SDL_DestroyWindow(window_);

	//Quit SDL subsystems
	SDL_Quit();
}

bool Application::InitWindow()
{
	// Initialize SDL 
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "%s: %s\n", "Couldn't initialize SDL", SDL_GetError());
		return false;
	}
	atexit(SDL_Quit);

	SDL_GL_LoadLibrary(nullptr); // Default OpenGL is fine.

								 // Request an OpenGL 4.3 context (should be core)
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

#ifdef HDR_FRAMEBUFFER
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 16);
#endif

	// Also request a depth buffer
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	// Create the window
	SDL_Window * window = SDL_CreateWindow(caption_.c_str(),
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		width_, height_, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

	if (window == nullptr) {
		fprintf(stderr, "%s: %s\n", "Couldn't set video mode", SDL_GetError());
		return false;
	}

	static SDL_GLContext maincontext = SDL_GL_CreateContext(window);
	if (maincontext == nullptr) {
		fprintf(stderr, "%s: %s\n", "Failed to create OpenGL context", SDL_GetError());
		return false;
	}

	// Initialize GLEW; this gives us access to OpenGL Extensions.
	glewInit();

	// Initialize ImGui; will allow us to edit variables in the application.
	//ImGui_ImplSdlGL3_Init(window);

	// Check OpenGL properties
	DebugHelper::StartupGLDiagnostics();
	DebugHelper::SetupGLDebugMessages();

	// Flip textures vertically so they don't end up upside-down.
	//stbi_set_flip_vertically_on_load(true);

	// 1 for v-sync
	SDL_GL_SetSwapInterval(1);

	/* Workaround for AMD. It might no longer be necessary, but I dunno if we
	* are ever going to remove it. (Consider it a piece of living history.)
	*/
	if (!glBindFragDataLocation)
	{
		glBindFragDataLocation = glBindFragDataLocationEXT;
	}
	this->window_ = window;
	return true;
}

void Application::Run()
{
	if (window_ == nullptr)
	{
		DebugHelper::NonFatalError("Window not initialized", "No window");
		return;
	}

	renderer_.InitGl();


	int w, h;
	SDL_GetWindowSize(window_, &w, &h);
	glm::vec3 cameraPosition(-70.0f, 50.0f, 70.0f);
	glm::vec3 cameraDirection = normalize(glm::vec3(0.0f) - cameraPosition);
	std::shared_ptr<Camera> example_camera = std::make_shared<Camera>(cameraPosition, cameraDirection, glm::radians(45.0f), w, h, 5.0f, 500.0f);
	std::shared_ptr<Light> example_light = std::make_shared<Light>();
	std::shared_ptr<Model> example_model = std::make_shared<Model>();
	example_model->LoadModelFromOBJ("../Assets/Models/BigSphere.obj");
	std::shared_ptr<Object> example_object = std::make_shared<Object>(example_model);

	

	std::shared_ptr<Scene> example_scene = std::make_shared<Scene>(example_camera);
	example_scene->lights.push_back(example_light);
	example_scene->objects.push_back(example_object);
	// application loop
	bool exit = false;
	while (!exit) {
		SDL_GetWindowSize(window_, &w, &h);
		example_scene->Update();
		renderer_.Display(window_, example_scene);

		SDL_GL_SwapWindow(window_);
		
		// Check events (keyboard among other)
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			// Allow ImGui to capture events.
			//ImGui_ImplSdlGL3_ProcessEvent(&event);

			// And do our own handling of events.
			if (event.type == SDL_QUIT || (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_ESCAPE)) {
				exit = true;
			}
		}
	}
}
