#pragma once
#ifndef  _UTILS_H
#define _UTILS_H

#include <string>

namespace Utils {
	inline std::string GetFileBaseName(std::string filename)
	{
		int pos = 0;
		for (unsigned int i = 0; i < filename.length(); i++)
			if (filename.c_str()[i] == '\\')
				pos = i + 1;

		return filename.substr(pos, filename.length());
	}

	inline std::string GetXmlFileTokenName(std::string filename)
	{
		std::string temp = GetFileBaseName(filename);

		return temp.replace(temp.rfind(".jack"), 6, "T.xml");
	}

	inline std::string GetXmlFileName(std::string filename)
	{
		std::string temp = GetFileBaseName(filename);

		return temp.replace(temp.rfind(".jack"), 5, ".xml");
	}
}
#endif // ! _UTILS_H