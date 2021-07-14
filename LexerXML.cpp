#include "stdafx.h"
#include "LexerXML.h"

Lexer::Lexer(const char* filename) :
	m_filename(filename)
{
	Tokenizer token(filename);
	FILE* arq = fopen(GetXmlFileName().c_str(), "w");
	printf("criando %s\n", GetXmlFileName().c_str());

	if (arq) {
		fputs("<tokens>\n", arq);
		while (token.GetNextToken().IsValid())
		{
			printf("<%s> %s </%s>\n", token.GetTypeName().c_str(), token.GetRealName().c_str(), token.GetTypeName().c_str());
			fprintf(arq, "<%s> %s </%s>\n", token.GetCurToken().GetTypeName().c_str(), token.GetCurToken().GetRealName().c_str(), token.GetCurToken().GetTypeName().c_str());
		}
		fputs("</tokens>\n", arq);
		fclose(arq);
	}
}

std::string Lexer::GetFileBaseName()
{
	int pos = 0;
	for (unsigned int i = 0; i < m_filename.length(); i++)
		if (m_filename.c_str()[i] == '\\')
			pos = i + 1;

	return m_filename.substr(pos, m_filename.length());
}

std::string Lexer::GetXmlFileName()
{
	std::string temp = GetFileBaseName();

	return temp.replace(temp.rfind(".jack"), 6, "T.xml");
}

Lexer::~Lexer()
{
}
