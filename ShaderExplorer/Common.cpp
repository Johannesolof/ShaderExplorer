#include "Common.h"
#include <fstream>
#include <cerrno>

namespace Common
{
	std::string GetFileContents(const char *filename)
	{
		std::ifstream in(filename, std::ios::in | std::ios::binary);
		if (in.good())
		{
			std::string contents;
			in.seekg(0, std::ios::end);
			contents.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&contents[0], contents.size());
			in.close();
			return(contents);
		}
		throw(errno);
	}
}