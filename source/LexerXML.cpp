#include "stdafx.h"
#include "LexerXML.h"
#include "utils.h"

Lexer::Lexer(const char* filename) :
	m_filename(filename)
{
	Tokenizer token(filename);
	FILE* arq = fopen(Utils::GetXmlFileTokenName(filename).c_str(), "w");
	printf("criando %s\n", Utils::GetXmlFileTokenName(filename).c_str());

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

Lexer::~Lexer()
{
}
