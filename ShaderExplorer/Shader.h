#pragma once
#include <GL/glew.h>
#include <string>

class Shader
{
public:
	GLuint program;
	
	Shader();
	~Shader();

	void LoadShader(std::string vert_file_path, std::string frag_file_path, bool allow_errors);
	std::string InfoLog(GLuint obj);

private:
	bool Compile(GLuint shader, bool allow_errors, std::string name = std::string());
	bool Link(GLuint shader_program, bool allow_errors);
};

