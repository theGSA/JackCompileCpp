#pragma once
#ifndef _TOKENS
#define _TOKENS


#include <string>

enum class TYPE
{
	T_SYMBOLS,
	T_KEYWORDS,
	T_IDENTIFIER,
	T_INT_CONST,
	T_STRING_CONST,
	T_UNDEFINED
};

enum class KWDTP //keyword type
{
	KCLASS,
	KCONSTRUCTOR,
	KFUNCTION,
	KMETHOD,
	KFIELD,
	KSTATIC,
	KVAR,
	KINT,
	KCHAR,
	KBOOLEAN,
	KVOID,
	KTRUE,
	KFALSE,
	KNULL,
	KTHIS,
	KLET,
	KDO,
	KIF,
	KELSE,
	KWHILE,
	KRETURN,
	KUNKNOWN
};

 
enum class SBLTP //symbols types
{

};
extern const char* keywords[];
extern const char* tipos[];
extern const char* simbolos[];

class Keyword
{
public:
	Keyword();
	~Keyword();
	Keyword(KWDTP type):
		m_type(type)
	{};
	KWDTP Get() { return m_type; };

	bool IsVarType() {
		return m_type == KWDTP::KINT || m_type == KWDTP::KBOOLEAN || m_type == KWDTP::KCHAR;
	};
	bool IsClassVarDec()
	{
		return m_type == KWDTP::KSTATIC || m_type == KWDTP::KFIELD;
	};
	bool IsSubroutine()
	{
		return m_type == KWDTP::KMETHOD || m_type == KWDTP::KFUNCTION || m_type == KWDTP::KCONSTRUCTOR;
	}
	bool IsStatement()
	{
		return m_type == KWDTP::KLET || m_type == KWDTP::KIF || m_type == KWDTP::KWHILE || m_type == KWDTP::KDO || m_type == KWDTP::KRETURN;
	}

	std::string GetTagXML() {
		return "<keyword> " + std::string(keywords[(int)m_type]) + " </keyword>";
	}
	bool IsKeywordConstant() {
		return m_type == KWDTP::KTRUE || m_type == KWDTP::KFALSE || m_type == KWDTP::KNULL || m_type == KWDTP::KTHIS;
	}

protected:
	//void SetType(KWDTP type) { m_type = type; };
private:
	KWDTP m_type;
};

class Token
{
public:
	std::string nometoken;
	Token();
	Token(std::string strname);
	//KWDTP Keyword() { return m_keyword; };
	bool IsValid() { return type != TYPE::T_UNDEFINED ? true : false; };
	TYPE GetType() {
		return type;
	};
	bool IsSymbol() { return TYPE::T_SYMBOLS == type; }
	bool IsTerm() { return type == TYPE::T_INT_CONST || type == TYPE::T_STRING_CONST || m_keyword.IsKeywordConstant() || type == TYPE::T_IDENTIFIER || IsOpUnary() || nometoken == "("; }
	bool IsOpUnary() { return nometoken == "~" || nometoken == "=" || nometoken == ">" || nometoken == "<"; }
	virtual std::string GetTokenName();
	virtual std::string GetRealName();
	virtual std::string GetTypeName();
	void dump()
	{
		printf("name(%s), token(%s), type(%d)\n", GetTokenName().c_str(), GetTypeName().c_str(), (int)type);
	}
	Keyword GetKeyword() { return m_keyword; }
	std::string GetTagXML() {
		return "<"+GetTypeName() + "> " + GetTokenName() + " </" + GetTypeName()+">";
	}
protected:

	bool IsSymbol(char ch);
	const char* ToSymbolName(const char* str);
	TYPE GetTokenType(const char* name);
private:
	TYPE type;
	//KWDTP m_keyword;
	Keyword m_keyword;
	KWDTP GetKeywordType();
	bool IsSymbol(const char* str);
	bool IsKeyword(const char* str);
	bool IsIdentifier(const char* str);
	bool IsIntConst(const char* str);
	bool IsStringConstant(const char* str);
};
#endif // !_TOKENS