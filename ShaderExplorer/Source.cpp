#include "SDL2/SDL.h"
#include "Application.h"
#include <iostream>


int main(int argc, char* argv[])
{
	Application app = Application("Test");
	app.InitWindow();
	app.Run();
	return 0;
}
                                                                               
int main2(int argc, char* argv[])
{
	auto up = Transform::WorldUp;
	glm::mat4 matrix( 1,  0,  0,  3, 
					  0,  1,  0,  7, 
		              0,  0,  1, 11, 
		              0,  0,  0,  1);
	Transform t(matrix);
	auto f = t.Forward();
	auto p = t.Position();
	auto r = t.Right();
	auto u = t.Up();
	auto rot = t.Rotation();
	t.Translate(p);
	return 0;
}