#pragma once
#ifndef LEXER_XML_
#define LEXER_XML
#include "Tokenizer.h"

class Lexer
{
public:
	Lexer(const char* filename);
	std::string GetFileBaseName();
	std::string GetXmlFileName();
	~Lexer();

private:
	std::string m_filename;

};

#endif // !LEXER_XML_

