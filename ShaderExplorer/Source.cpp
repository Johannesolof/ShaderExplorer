#include "SDL2/SDL.h"
#include "Application.h"


int main(int argc, char* argv[])
{
	Application app = Application();
	app.InitWindow("Test");
	app.Run();
	return 0;
}
