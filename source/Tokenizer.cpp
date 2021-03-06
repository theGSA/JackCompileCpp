#include <stdio.h>
#include "stdafx.h"
#include "Tokenizer.h"





Tokenizer::Tokenizer(const char* nomearq):
	m_filename(nomearq),
	m_Col(0),
	m_Lin(0)
{
	Load();
}


void Tokenizer::FormatTokenName(const char* str, char* dest)
{
	if (str[0] == '"') {
		if (str[strlen(str) - 1] == '\"')
		{
			strncpy(dest, str, strlen(str));
		}
		else {
			sprintf(dest, "%s %s\"", str, strtok(NULL, "\""));
		}
	}
	else
		strcpy(dest, str);
}

char Tokenizer::GetNextChar(/*FILE* const arq*/)
{
	
	const char c = getc(m_arq);
	//return c;
	return c == EOF ? EOF : ungetc(c, m_arq);
}



void Tokenizer::CreateXML()
{
	FILE* arq;
	char* buffer = _strdup(m_filename.c_str());
	char* aux = strstr(buffer, ".jack");

	if (aux == 0) {
		printf("inconsistencia no nome do arquivo (%s)\n", m_filename.c_str());
		return;
	}
	else
		strncpy(aux, "T.xml", 6);

	arq = fopen(buffer, "w");

	if (!arq) {
		printf("nao foi possivel criar o arquivo (%s)\n", buffer);
		return;
	}

	printf("criando o xml (%s)\n", buffer);
	free(buffer);
	fputs("<tokens>\n", arq);
	for (unsigned int i = 0; i < m_tokens.size(); i++) {
		//fprintf(arq,"<%s> %s </%s>\n", tipos[(int)m_tokens.at(i).type],ToSymbolName(m_tokens.at(i).nometoken.c_str()), tipos[(int)m_tokens.at(i).type]);
		//printf("<%s> %s </%s>\n", tipos[(int)m_tokens.at(i).type],ToSymbolName(m_tokens.at(i).nometoken.c_str()), tipos[(int)m_tokens.at(i).type]);
	}
	fputs("</tokens>\n", arq);
	fclose(arq);
}

Token Tokenizer::GetNextToken()
{
	char ch;
	int contar = 0;
	char* buffer;
	bool comsimples = false;
	bool commultiplo = false;
	bool aspas = false;
	char* aux;
	char ultchar = 0;

	buffer = (char*)malloc(2048 * sizeof(char) + 1);
	if (!buffer)
	{
		printf("n?o foi possivel alocar memoria\n");
		return Token();
	}

	aux = buffer;
	while ((ch = getc(m_arq)) != EOF)
	{
		if (ch == ' ')
		{
			while ((ch = getc(m_arq)) != EOF && ch == ' ');
		}
		if (ch == '/' || comsimples || commultiplo)
		{
			char ant = ch;
			char tch = GetNextChar();

			if (tch == '/' || tch == '*') {
				if (tch == '/') comsimples = true;
				else if (tch == '*')commultiplo = true;
			}
		}

		if (ch == '\n') {
			if (comsimples)
				comsimples = false;
			m_Lin++;
			m_Col = 0;
		}

		if (commultiplo && (ch == '*')) {
			char tch = GetNextChar();
			if (tch == '/') {
				commultiplo = comsimples = false;
				ch = getc(m_arq);
				ch = getc(m_arq);
			}
		}

		if (!comsimples && !commultiplo && ch != '\n' && ch != '\r' && ch != '\t') {
			*aux++ = ch;
			if (ch == '\"') {
				while ((ch = getc(m_arq)) != EOF && ch != '\"')
					*aux++ = ch;
				*aux++ = ch;
			}
			if (ch != EOF && (ch == '\"' || GetNextChar() == ' ' || IsSymbol(GetNextChar()) || IsSymbol(ch)))
			{
				*aux = 0;
				Token temp = Token(buffer);
				temp.m_line = m_Lin + 1;
				temp.m_collumn = m_Col++;
				m_currentToken = temp;
				
				PeekToken();
				free(buffer);
				return m_currentToken;
			}
		}
		contar++;
	}
	
	return Token();
}

void Tokenizer::PeekToken()
{
	std::string peek;
	int pos = ftell(m_arq);
	int tline = m_Lin;
	int tcol = m_Col;

	Token aux = m_currentToken;
	peek = GetNextToken().GetRealName();
	m_currentToken = aux;

	fseek(m_arq, pos, SEEK_SET);
	m_Lin = tline;
	m_Col = tcol;
	m_peekToken = Token(peek.c_str());
	//return m_peekToken;
}
void Tokenizer::Load()
{

	if (!strstr(m_filename.c_str(), ".jack")) {
		printf("n?o pode carregar o arquivo (%s)! n?o ? um .jack\n", m_filename.c_str());
		return;
	}
	else {
		m_arq = fopen(m_filename.c_str(), "rb");
		if (!m_arq)
		{
			printf("o arquivo (%s) n?o existe\n", m_filename.c_str());
			return;
		}
		else {
			printf("arquivo (%s) carregado com sucesso\n", m_filename.c_str());
			LexicalAnalize();
		}
	}
}


