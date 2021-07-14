
#pragma once
#include "stdafx.h"

#include "token.h"
#include "Tokenizer.h"
const char* keywords[] =
{
	"class",
	"constructor",
	"function",
	"method",
	"field",
	"static",
	"var",
	"int",
	"char",
	"boolean",
	"void",
	"true",
	"false",
	"null",
	"this",
	"let",
	"do",
	"if",
	"else",
	"while",
	"return",
	NULL,
};

const char* tipos[] =
{
	"symbol",
	"keyword",
	"identifier",
	"integerConstant",
	"stringConstant",
	"undefined",
	NULL
};

const char* simbolos[] = { "{","}","(",")","[","]",".",",",";","+","-","","*","&","|",",","<",">","=","~","/", NULL };

Token::Token():
	nometoken(""),
	type(TYPE::T_UNDEFINED),
	m_keyword(Keyword())
{
}
Token::Token(std::string strname)
{
	//Tokenizer temp;
	nometoken = strname;
	type = GetTokenType(strname.c_str());
	m_keyword = Keyword(GetKeywordType());
}

KWDTP Token::GetKeywordType()
{

	for (size_t i = 0; NULL != keywords[i]; i++)
	{
		if (strcmp(keywords[i], nometoken.c_str()) == 0)
			return (KWDTP)i;
	}
	return KWDTP::KUNKNOWN;
}
std::string Token::GetTokenName()
{
	return std::string(ToSymbolName(nometoken.c_str()));
}

std::string Token::GetRealName()
{
	return nometoken;
}

std::string Token::GetTypeName()
{
	return std::string(tipos[(int)type]);
}

TYPE Token::GetTokenType(const char* str)
{
	if (IsKeyword(str))					return TYPE::T_KEYWORDS;
	else if (IsSymbol(str))				return TYPE::T_SYMBOLS;
	else if (IsIntConst(str))	return TYPE::T_INT_CONST;
	else if (IsStringConstant(str))	return TYPE::T_STRING_CONST;
	else if (IsIdentifier(str))		return TYPE::T_IDENTIFIER;

	return TYPE::T_UNDEFINED;
}

bool Token::IsOp()
{
	std::string  op = "+-*/&|<>=";

	for (int i = 0; op[i] != 0; i++) {
		if (op.substr(i, 1) == GetRealName())
			return true;
	}
	return false;
}

const char* Token::ToSymbolName(const char* str)
{
	char buffer[2058] = "";

	if (strcmp(str, "<") == 0) return "&lt;";
	else if (strcmp(str, ">") == 0) return "&gt;";
	else if (strcmp(str, "&") == 0) return "&amp;";
	else if (IsStringConstant(str)) {
		strncpy(buffer, str + 1, strlen(str) - 2);
		return buffer;
	}
	return str;
}

bool Token::IsSymbol(const char* str)
{
	for (int i = 0; simbolos[i] != NULL; i++)
		if (strcmp(simbolos[i], str) == 0)
			return true;
	return false;
}
bool Token::IsSymbol(char ch)
{
	char buffer[2];

	if (ch > 0) {
		sprintf(buffer, "%c", ch);
		return IsSymbol(buffer);
	}
	else
		return false;
}
bool Token::IsKeyword(const char* str)
{
	for (int i = 0; keywords[i] != NULL; i++)
		if (strcmp(keywords[i], str) == 0)
			return true;
	return false;
}
bool Token::IsIdentifier(const char* str)
{
	/*if (m_tokens.size() > 0 && ((m_tokens.back().type == TYPE::T_KEYWORDS && ETipoVariavel(m_tokens.back().nometoken.c_str()) ||
		m_tokens.back().type == TYPE::T_SYMBOLS ||
		m_tokens.back().type == TYPE::T_IDENTIFIER))) {
		return true;
	}*/
	/*for (unsigned int i = 0; i < m_tokens.size(); i++) {
		if (strcmp(m_tokens.at(i).nometoken.c_str(), str) == 0)
		{
			return m_tokens[i].type == TYPE::T_IDENTIFIER ||
				m_tokens[i].type == TYPE::T_SYMBOLS && m_tokens[i].nometoken[0] == ',';
		}
	}*/
	return true;
}

bool Token::IsStringConstant(const char* str)
{
	return str[0] == '"';
}
bool Token::IsIntConst(const char* str)
{
	for (int i = 0; str[i] != 0; i++)
		if (str[i] < '0' || str[i] > '9')
			return false;

	return true;
}


//////////////////
Keyword::Keyword() :
	m_type(KWDTP::KUNKNOWN)
{

}

Keyword::~Keyword()
{
}