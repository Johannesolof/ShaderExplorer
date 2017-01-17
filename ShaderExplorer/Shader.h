#pragma once
#include <GL/glew.h>
#include <string>
#include <glm/glm.hpp>

class Shader
{
public:
	
	Shader();
	~Shader();

	void LoadShader(std::string vert_file_path, std::string frag_file_path, bool allow_errors);
	std::string InfoLog(GLuint obj);
	
	void UseProgram() const;

	void SetUniformSlow(const char* name, const glm::mat4& matrix) const;
	void SetUniformSlow(const char* name, const float value) const;
	void SetUniformSlow(const char* name, const GLint value) const;
	void SetUniformSlow(const char* name, const glm::vec3& value) const;

private:
	GLuint program_;
	
	bool Compile(GLuint shader, bool allow_errors, std::string name = std::string());
	bool Link(GLuint shader_program, bool allow_errors);
};

