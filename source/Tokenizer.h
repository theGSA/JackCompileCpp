#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "token.h"



class Tokenizer: public Token
{
	public:
		Tokenizer():
			m_arq(NULL)
		{};
		Tokenizer(const char* filename);
		virtual ~Tokenizer() {};
		Token GetNextToken();
		//TYPE GetTokenType(const char* str);

		Token GetCurToken() { return m_currentToken; };
		std::string GetTokenName() override { return std::string(ToSymbolName(m_currentToken.nometoken.c_str())); };
		std::string GetRealName() override { return m_currentToken.nometoken; };
		std::string GetTypeName() override { return std::string(tipos[(int)m_currentToken.GetType()]);};
		TYPE GetType() { return m_currentToken.GetType(); };
	private:
		Token m_currentToken;
		std::string m_filename;
		FILE* m_arq;
		//char* m_stringfy;

		std::vector <Token> m_tokens;
		void Load();
		void LexicalAnalize() {};
		void CreateXML();

		//const char* ToSymbolName(const char* str);
		void FormatTokenName(const char* str, char* dest);
		char GetNextChar(/*FILE* const arq*/);

};

