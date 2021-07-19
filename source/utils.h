#pragma once
#ifndef  _UTILS_H
#define _UTILS_H

#include <string>

namespace Utils {
	inline std::string GetFileBaseName(const std::string filename)
	{
		int pos = 0;
		//std::string aux(filename.c_str());

		for (unsigned int i = 0; i < filename.length(); i++)
			if (filename.c_str()[i] == '\\')
				pos = i + 1;

		return std::string(filename.c_str()).substr(pos, filename.length());
	}

	inline std::string GetXmlFileTokenName(const std::string filename)
	{
		std::string temp = GetFileBaseName(filename);

		return temp.replace(temp.rfind(".jack"), 7, "T.xml");
	}

	inline std::string GetXmlFileName(const std::string filename)
	{
		std::string temp = GetFileBaseName(filename);

		return temp.replace(temp.rfind(".jack"), 6, ".xml");
	}
}
#endif // ! _UTILS_H