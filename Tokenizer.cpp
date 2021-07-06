#include <stdio.h>
#include "stdafx.h"
#include "Tokenizer.h"


const char* keywords[] =
{
	"class",
	"int",
	"boolean",
	"method",
	"function",
	"constructor", 
	"this",
	"char",
	"void",
	"var",
	"static",
	"let",
	"field",
	"do",
	"while",
	"if",
	"else",
	"true",
	"return",
	"false",
	"null",
	NULL,
};

const char* tipos[] =
{
	"symbol",
	"keyword",
	"identifier",
	"integerConstant",
	"stringConstant",
	"none",
	NULL
};


Tokenizer::Tokenizer(const char* nomearq):
	m_nomearq(nomearq)
{
	DeleteCaminho();
	Carregar();
}

bool Tokenizer::ECaracterAceito(char ch)
{
	char aceitos[] = "<> (){}[],.;\"'|\\:-+=?/*&%";

	for (int i = 0; aceitos[i] != 0; i++)
		if (ch == aceitos[i])
			return true;
	return false;
}

bool ETipoVariavel(const char* str)
{
	char* tipos[] = {"class","var", "int", "char", "boolean","do", "void", NULL};

	for (int i = 0; tipos[i] != NULL; i++)
		if (strcmp(str, tipos[i]) == 0)
			return true;

	return false;
}

bool Tokenizer::ESimbolo(const char* str)
{
	const char* simbolos[] = {"{", "}", "(", ")","]", "[", "<", ">", "=","!=", "==", "<=", ">=", ".", ";", ",", "*", "-", "+", "/", "|", "&", NULL};
	
	for (int i = 0; simbolos[i] != NULL; i++)
		if (strcmp(simbolos[i], str) == 0)
			return true;
	
	return false;
}

bool Tokenizer::EKeyword(const char* str)
{
	for (int i = 0; keywords[i] != NULL; i++)
		if (strcmp(keywords[i], str) == 0)
			return true;
	return false;
}

bool Tokenizer::EIdentificador(const char* str)
{
	if (m_tokens.back().type == TIPOS::T_KEYWORDS && ETipoVariavel(m_tokens.back().nome)||
		m_tokens.back().type == TIPOS::T_SYMBOLS && m_tokens.back().nome[0] == ',' ||
		m_tokens.back().type == TIPOS::T_SYMBOLS && m_tokens.back().nome[0] == '.' ||
		m_tokens.back().type == TIPOS::T_IDENTIFIER) {
		return true;
	}
	for (unsigned int i = 0; i < m_tokens.size(); i++) {
		if (strcmp(m_tokens.at(i).nome, str) == 0)
		{
			return m_tokens[i].type == TIPOS::T_IDENTIFIER ||
				   m_tokens[i].type == TIPOS::T_SYMBOLS && m_tokens[i].nome[0] == ',';
		}
	}
	return false;
}

bool Tokenizer::EDelimitador(char ch)
{
	char delimitador[] = "(){}[],.;-";

	for (int i = 0; delimitador[i] != 0; i++)
		if (ch == delimitador[i])
			return true;
	return false;

}

TIPOS Tokenizer::ObterTipo(const char* str)
{
	if (EKeyword(str))					return TIPOS::T_KEYWORDS;
	else if (ESimbolo(str))				return TIPOS::T_SYMBOLS;
	else if (EUmInteiroConstant(str))	return TIPOS::T_INT_CONST;
	else if (EUmStringConstant(str))	return TIPOS::T_STRING_CONST;
	else if (EIdentificador(str))		return TIPOS::T_IDENTIFIER;
	
	return TIPOS::T_NONE;
}

bool Tokenizer::EUmStringConstant(const char* str)
{
	return str[0] == '"';
}

void Tokenizer::CheckSeEStringConstant(const char* str, char* dest)
{
	char* buffer = (char*)malloc(2048 * sizeof(char));

	//buffer[0] = 0;
	if (buffer != 0) {
		if (str[0] == '"') {
			if (str[strlen(str) - 1] == '\"')
			{
				strncpy(dest, str, strlen(str));
			}
			else {
				sprintf(buffer, "%s %s\"", str, strtok(NULL, "\""));
				strcpy(dest, buffer);
			}
		}
		else
			strcpy(dest, str);
		free(buffer);
	}
	else {
		printf("não foi possivel alocar memoria\n");
	}
}

char Tokenizer::ObterProximoChar(FILE* const arq)
{
	const char c = getc(arq);
	return c == EOF ? EOF : ungetc(c, arq);
}

void Tokenizer::AnalizeLexica()
{
	char ch;
	int contar = 0;
	char* buffer;
	bool comsimples = false;
	bool commultiplo = false;
	bool aspas = false;
	char* aux;
	char ultchar = 0;

	Token temp = {};

	fseek(m_arq, 0L, SEEK_END);
	buffer = (char*)malloc(ftell(m_arq) * sizeof(char) + 1);
	if (!buffer)
	{
		printf("não foi possivel alocar memoria\n");
		return;
	}
	aux = buffer;

	fseek(m_arq, 0L, SEEK_SET);
	while ((ch = getc(m_arq)) != EOF)
	{
		if (ch == '/' || comsimples || commultiplo)
		{
			char tch = ObterProximoChar(m_arq);

			if ( tch == '/' || tch == '*') {
				if (tch == '/') comsimples = true;
				else if (tch == '*')commultiplo = true;
			}
		}

		if (ch == '\n' && comsimples) {
			comsimples = false;
		}

		if (commultiplo && (ch == '*')) {
			char tch = ObterProximoChar(m_arq);
			if (tch == '/') {
				commultiplo = comsimples = false;
				ch = getc(m_arq);
				ch = getc(m_arq);
			}
		}
		
		if (!comsimples && !commultiplo && ch != '\n' && ch != 13 && ch != '\t' &&/* (isalnum(ch) || ECaracterAceito(ch)) &&*/ ((ultchar != ' ' && (ch != ' ' || ch == ' ')) || (ultchar == ' ' && ch != ' '))) {
			if ((EDelimitador(ch) && ultchar != ' ') || (EDelimitador(ultchar) && ch != ' '))
				*aux++ = ' ';
			*aux++ = ch;
			//printf("insere: (%c)(%d)\n", ch, ch);
			ultchar = ch;
		}
		contar++;
	}
	*aux = '\0';
	m_stringfy = _strdup(buffer);

	//printf("%s\n", m_stringfy);
	aux = strtok(buffer, " ");
	while (aux != NULL)
	{
		CheckSeEStringConstant(aux, temp.nome);
		temp.type = ObterTipo(aux);
		m_tokens.push_back(temp);
		aux = strtok(NULL, " ");
	}
	CriarXML();
	free(buffer);
}

void Tokenizer::CriarXML()
{
	FILE* arq;
	char* buffer = _strdup(m_nomearq.c_str());
	char* aux = strstr(buffer, ".jack");

	if (aux == 0) {
		printf("inconsistencia no nome do arquivo (%s)\n", m_nomearq.c_str());
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
		fprintf(arq,"<%s> %s </%s>\n", tipos[(int)m_tokens.at(i).type],ParaSimbolo(m_tokens.at(i).nome), tipos[(int)m_tokens.at(i).type]);
		printf("<%s> %s </%s>\n", tipos[(int)m_tokens.at(i).type],ParaSimbolo(m_tokens.at(i).nome), tipos[(int)m_tokens.at(i).type]);
	}
	fputs("</tokens>\n", arq);
	fclose(arq);
}
void Tokenizer::DeleteCaminho()
{
	int pos = 0;
	for (unsigned int i = 0; i < m_nomearq.length(); i++)
		if (m_nomearq.c_str()[i] == '\\')
			pos = i+1;

	m_nomearq = m_nomearq.substr(pos, m_nomearq.length());
}
const char* Tokenizer::ParaSimbolo(const char* str)
{
	char buffer[2058] = "";

	if (strcmp(str, "<") == 0) return "&lt;";
	else if (strcmp(str, ">") == 0) return "&gt;";
	else if (strcmp(str, "&") == 0) return "&amp;";
	else if (EUmStringConstant(str)) {
		strncpy(buffer, str + 1, strlen(str) - 2);
		return buffer;
	}
	return str;
}
bool Tokenizer::EUmInteiroConstant(const char* str)
{
	for (int i = 0; str[i] != 0; i++)
		if (str[i] < '0' || str[i] > '9')
			return false;

	return true;
}
void Tokenizer::Carregar()
{

	if (!strstr(m_nomearq.c_str(), ".jack")) {
		printf("não pode carregar o arquivo (%s)! não é um .jack\n", m_nomearq.c_str());
		return;
	}
	else {
		m_arq = fopen(m_nomearq.c_str(), "rb");
		if (!m_arq)
		{
			printf("o arquivo (%s) não existe\n", m_nomearq.c_str());
			return;
		}
		else {
			printf("arquivo (%s) carregado com sucesso\n", m_nomearq.c_str());
			AnalizeLexica();
		}
	}
}
