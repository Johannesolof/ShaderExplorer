#include "Shader.h"
#include "Common.h"
#include "DebugHelper.h"


Shader::Shader()
{
	program_ = 0;
}


Shader::~Shader()
{
}

void Shader::LoadShader(std::string vert_file_path, std::string frag_file_path, bool allow_errors)
{
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	std::string vertexSrc = Common::GetFileContents(vert_file_path.c_str());
	std::string fragmentSrc = Common::GetFileContents(frag_file_path.c_str());

	const char *vs = vertexSrc.c_str();
	const char *fs = fragmentSrc.c_str();

	glShaderSource(vertexShader, 1, &vs, nullptr);
	glShaderSource(fragmentShader, 1, &fs, nullptr);

	if (!Compile(vertexShader, allow_errors, "Vertex Shader") || !Compile(fragmentShader, allow_errors, "Fragment Shader"))
	{
		program_ = 0;
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		return;
	}

	// Create a program_ object and attach the two shaders we have compiled, the program_ object contains
	// both vertex and fragment shaders as well as information about uniforms and attributes common to both.
	program_ = glCreateProgram();
	glAttachShader(program_, fragmentShader);
	glAttachShader(program_, vertexShader);

	// Now that the fragment and vertex shader has been attached, we no longer need these two separate objects and should delete them.
	// The attachment to the shader program_ will keep them alive, as long as we keep the shader_program.
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	if (!allow_errors) CHECK_GL_ERROR();

	if (!Link(program_, allow_errors)) return;

	// Link the different shaders that are bound to this program_, this creates a final shader that 
	// we can use to render geometry with.
	glLinkProgram(program_);

	// Check for linker errors, many errors, such as mismatched in and out variables between 
	// vertex/fragment shaders,  do not appear before linking.
	{
		GLint linkOk = 0;
		glGetProgramiv(program_, GL_LINK_STATUS, &linkOk);
		if (!linkOk)
		{
			std::string err = InfoLog(program_);
			DebugHelper::FatalError(err);
		}
	}
}

std::string Shader::InfoLog(GLuint obj)
{
	int logLength = 0;
	int charsWritten = 0;
	char *tmpLog;
	std::string log;

	glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &logLength);

	if (logLength > 0) {
		tmpLog = new char[logLength];
		glGetShaderInfoLog(obj, logLength, &charsWritten, tmpLog);
		log = tmpLog;
		delete[] tmpLog;
	}

	return log;
}

void Shader::UseProgram() const
{
	glUseProgram(program_);
}

void Shader::SetUniformSlow(const char* name, const glm::mat4& matrix) const
{
	glUniformMatrix4fv(glGetUniformLocation(program_, name), 1, false, &matrix[0].x);
}

void Shader::SetUniformSlow(const char* name, const float value) const
{
	glUniform1f(glGetUniformLocation(program_, name), value);
}

void Shader::SetUniformSlow(const char* name, const GLint value) const
{
	int loc = glGetUniformLocation(program_, name);
	glUniform1i(loc, value);
}

void Shader::SetUniformSlow(const char* name, const glm::vec3& value) const
{
	glUniform3fv(glGetUniformLocation(program_, name), 1, &value.x);
}

bool Shader::Compile(GLuint shader, bool allow_errors, std::string name)
{
	// Compile the shader, translates into internal representation and checks for errors.
	glCompileShader(shader);
	int compileOK;
	// check for compiler errors in vertex shader.
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileOK);
	if (compileOK != GL_TRUE) {
		std::string err = InfoLog(shader);
		if (allow_errors) {
			DebugHelper::NonFatalError(err, name);
		}
		else {
			DebugHelper::FatalError(err, name);
		}
	}
	return compileOK == GL_TRUE;
}

bool Shader::Link(GLuint shader_program, bool allow_errors)
{
	glLinkProgram(shader_program);
	GLint linkOk = 0;
	glGetProgramiv(shader_program, GL_LINK_STATUS, &linkOk);
	if (linkOk != GL_TRUE)
	{
		std::string err = InfoLog(shader_program);
		if (allow_errors) {
			DebugHelper::NonFatalError(err, "Linking");
		}
		else {
			DebugHelper::FatalError(err, "Linking");
		}
	}
	return linkOk == GL_TRUE;
}