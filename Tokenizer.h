#pragma once

#include <iostream>
#include <string>
#include <vector>


enum class TIPOS
{
	T_SYMBOLS,
	T_KEYWORDS,
	T_IDENTIFIER,
	T_INT_CONST,
	T_STRING_CONST,
	T_NONE,
};


typedef struct
{
	char nometoken[256];
	TIPOS type;
}Token;


class Tokenizer
{
	public:
		Tokenizer(const char* nomearq);
	private:
		std::string m_nomearq;
		FILE* m_arq;
		char* m_stringfy;
		std::vector <Token> m_tokens;
		void Carregar();
		void AnalizeLexica();
		//bool ECaracterAceito(char ch);
		bool ESimbolo(const char* str);
		bool EKeyword(const char* str);
		bool EIdentificador(const char* str);
		bool EDelimitador(char ch);
		void CriarXML();
		void DeleteCaminho();
		const char* ParaSimbolo(const char* str);
		bool EUmInteiroConstant(const char* str);
		bool EUmStringConstant(const char* str);
		void FormatarNomeDoToken(const char* str, char* dest);
		char ObterProximoChar(FILE* const arq);
		TIPOS ObterTipo(const char* str);
};

