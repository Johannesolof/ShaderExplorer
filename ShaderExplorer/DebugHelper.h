#pragma once

#define CHECK_GL_ERROR() { DebugHelper::CheckGLError(__FILE__, __LINE__) && (__debugbreak(), 1); }

#include <string>

namespace DebugHelper
{

#ifndef _WIN32
#	define __debugbreak() assert(false)
#endif

	/**
	* Internal function used by macro CHECK_GL_ERROR, use that instead.
	*/
	bool CheckGLError(const char *file, int line);

	/**
	* Print GL version/vendor and renderer. Ensure that we've got OpenGL 3.0
	* (GLEW_VERSION_3_0). Bail if we don't.
	*/
	void StartupGLDiagnostics();

	/**
	* Initialize OpenGL debug messages.
	*/
	void SetupGLDebugMessages();

	/**
	* Error reporting function
	*/
	void FatalError(std::string error_string, std::string title = std::string());
	void NonFatalError(std::string error_string, std::string title = std::string());
};

