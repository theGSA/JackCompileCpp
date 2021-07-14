#include "stdafx.h"
#include "Sintaxer.h"

int count = 0;
Sintaxer::Sintaxer():
	m_output(NULL),
	tokenizer(NULL)
{
}

Sintaxer::Sintaxer(std::string filename)
{
	m_output = fopen("output.xml", "w");
	tokenizer = new Tokenizer(filename.c_str());
	tokenizer->GetNextToken();
	CompileClass();
}

Sintaxer::~Sintaxer()
{
	fclose(m_output);
	delete tokenizer;
}

void Sintaxer::CompileClass()
{
	const char* classTemplate[] = { "class", "className" ,"{", "classVarDec", "subroutineDec","}", NULL };
	int i = 0;


	//while (classTemplate[i++] != NULL)
	//{
	WriteFile("<class>");
	if (tokenizer->GetCurToken().GetKeyword().Get() == KWDTP::KCLASS)
	{
		WriteFile(tokenizer->GetCurToken().GetTagXML());
		tokenizer->GetNextToken();

		if (tokenizer->GetType() == TYPE::T_IDENTIFIER)
		{
			WriteFile(tokenizer->GetCurToken().GetTagXML());
			tokenizer->GetNextToken();
			i++;
			if (strcmp(tokenizer->GetRealName().c_str(), "{") == 0)
			{
				WriteFile(tokenizer->GetCurToken().GetTagXML());
				CompileClassVarDec();
				CompileSubroutine();
				WriteFile(tokenizer->GetCurToken().GetTagXML());
			}
			else
			{
				printf("simbolo (%s) inexperado\n", tokenizer->GetTypeName().c_str());
				return;
			}
		}
		else
		{
			printf("simbolo (%s) inexperado\n", tokenizer->GetTypeName().c_str());
			return;
		}
	}
	else {
		printf("%s nao é uma classe\n", tokenizer->GetTokenName().c_str());
		return;
	}
	//}
	WriteFile("</class>");
	fclose(m_output);
}

void Sintaxer::CompileClassVarDec()
{
	//( 'static' | 'field' ) type varName ( ',' varName)* ';'
	
	while (tokenizer->GetNextToken().GetKeyword().IsClassVarDec())
	{
		WriteFile("<classVarDec>");
		WriteFile(tokenizer->GetCurToken().GetTagXML());

		if (tokenizer->GetNextToken().GetKeyword().IsVarType() || tokenizer->GetCurToken().GetType() == TYPE::T_IDENTIFIER) {
			WriteFile(tokenizer->GetCurToken().GetTagXML());
			if (tokenizer->GetNextToken().GetType() == TYPE::T_IDENTIFIER) {
				WriteFile(tokenizer->GetCurToken().GetTagXML());
				if (tokenizer->GetNextToken().GetType() == TYPE::T_SYMBOLS) {
					if (tokenizer->GetCurToken().GetRealName() == ";") {
						WriteFile(tokenizer->GetCurToken().GetTagXML());
					}
				}
			}
			else {
				//to do
				printf("esperado um identificador\n");
				return;
			}
		}
		WriteFile("</classVarDec>");
	}
}

void Sintaxer::CompileSubroutine()
{	
	"('constructor' | 'function' | 'method') ('void' | type) subroutineName '(' parameterList ')' subroutineBody";

	//tokenizer->GetNextToken();
		//tokenizer->dump();
	while (tokenizer->GetCurToken().GetKeyword().IsSubroutine())
	{
		WriteFile("<subroutineDec>");
		if (tokenizer->GetCurToken().GetKeyword().Get() == KWDTP::KCONSTRUCTOR)
		{
			WriteFile(tokenizer->GetCurToken().GetTagXML());
			tokenizer->GetNextToken();
			if (tokenizer->GetCurToken().GetType() == TYPE::T_IDENTIFIER)
			{
				WriteFile(tokenizer->GetCurToken().GetTagXML());
				tokenizer->GetNextToken();
				if (tokenizer->GetCurToken().GetType() == TYPE::T_IDENTIFIER) {
					WriteFile(tokenizer->GetCurToken().GetTagXML());
					tokenizer->GetNextToken();
					if (tokenizer->GetTokenName() == "(")
					{
						WriteFile(tokenizer->GetCurToken().GetTagXML());
						CompileParameterList();
						if (tokenizer->GetTokenName() == ")")
						{
							WriteFile(tokenizer->GetCurToken().GetTagXML());
							CompileSubroutineBody();
						}
					}
				}
			}
		}
		else if (tokenizer->GetCurToken().GetKeyword().Get() == KWDTP::KFUNCTION)
		{
			WriteFile(tokenizer->GetCurToken().GetTagXML());
			tokenizer->GetNextToken();
		}
		else if (tokenizer->GetCurToken().GetKeyword().Get() == KWDTP::KMETHOD)
		{
			printf("compile method\n");
			WriteFile(tokenizer->GetCurToken().GetTagXML());
			if (tokenizer->GetNextToken().GetKeyword().Get() == KWDTP::KVOID)
			{
				WriteFile(tokenizer->GetCurToken().GetTagXML());
				if (tokenizer->GetNextToken().GetType() == TYPE::T_IDENTIFIER)
				{
					WriteFile(tokenizer->GetCurToken().GetTagXML());
					if (tokenizer->GetNextToken().GetTokenName() == "(")
					{
						WriteFile(tokenizer->GetCurToken().GetTagXML());
						CompileParameterList();
						if (tokenizer->GetTokenName() == ")")
						{
							WriteFile(tokenizer->GetCurToken().GetTagXML());
							CompileSubroutineBody();
						}
					}
				}
			}
		}
		WriteFile("</subroutineDec>");
		tokenizer->GetNextToken();
	}
	
}

void Sintaxer::CompileSubroutineBody()
{
	tokenizer->GetNextToken();
	WriteFile("<subroutineBody>");
	if (tokenizer->GetCurToken().GetTokenName() == "{")
	{
		WriteFile(tokenizer->GetCurToken().GetTagXML());
		tokenizer->GetNextToken();
		while (true)
		{
			if (tokenizer->GetCurToken().GetKeyword().IsStatement())
				CompileStatements();
			else if (tokenizer->GetCurToken().GetKeyword().Get() == KWDTP::KVAR)
				CompileVarDec();
			else if (tokenizer->GetNextToken().GetTokenName() == "}") {
				WriteFile(tokenizer->GetCurToken().GetTagXML());
				break;
			}
		}
	}
	WriteFile("</subroutineBody>");
}

void Sintaxer::CompileParameterList()
{
	//tokenizer->GetNextToken();
	WriteFile("<parameterList>");
	while (tokenizer->GetNextToken().IsValid() && tokenizer->GetCurToken().GetType() == TYPE::T_IDENTIFIER && tokenizer->GetCurToken().GetTokenName() != ")")
	{
		WriteFile(tokenizer->GetCurToken().GetTagXML());
		if (tokenizer->GetNextToken().GetTokenName() == ",")
		{
			WriteFile(tokenizer->GetCurToken().GetTagXML());
			break;
		}
		else {

		}
	}
	WriteFile("</parameterList>");
	//if(tokenizer->GetCurToken().GetType() == KWDTP::)
}

void Sintaxer::CompileVarDec()
{
	while (tokenizer->GetCurToken().GetKeyword().Get() == KWDTP::KVAR)
	{
		WriteFile("<varDec>");
		WriteFile(tokenizer->GetCurToken().GetTagXML());
		if (tokenizer->GetNextToken().GetKeyword().IsVarType()) {
			WriteFile(tokenizer->GetCurToken().GetTagXML());
			if (tokenizer->GetNextToken().GetType() == TYPE::T_IDENTIFIER)
			{
				WriteFile(tokenizer->GetCurToken().GetTagXML());
				if (tokenizer->GetNextToken().GetTokenName() == ";") {
					WriteFile(tokenizer->GetCurToken().GetTagXML());
					tokenizer->GetNextToken();
					//tokenizer->GetCurToken().dump();
				}
			}
		}
		WriteFile("</varDec>");
	}
}
void Sintaxer::CompileStatements()
{
	//if (tokenizer->GetCurToken().GetKeyword().IsStatement())
	//{
		WriteFile("<statements>");
			tokenizer->GetCurToken().dump();
		while (tokenizer->GetCurToken().GetKeyword().IsStatement()) {
			if (tokenizer->GetCurToken().GetKeyword().Get() == KWDTP::KLET)
				CompileLet();
			else if (tokenizer->GetCurToken().GetKeyword().Get() == KWDTP::KIF)
				CompileIf();
			else if (tokenizer->GetCurToken().GetKeyword().Get() == KWDTP::KWHILE)
				CompileWhile();
			else if (tokenizer->GetCurToken().GetKeyword().Get() == KWDTP::KDO)
				CompileDo();
			else if (tokenizer->GetCurToken().GetKeyword().Get() == KWDTP::KRETURN)
				CompileReturn();
			//tokenizer->GetNextToken();
		}
		WriteFile("</statements>");
	//}
}

void Sintaxer::CompileDo()
{
	WriteFile("<doStatement>");
	WriteFile(tokenizer->GetCurToken().GetTagXML());
	if (tokenizer->GetNextToken().GetType() == TYPE::T_IDENTIFIER)
	{
		WriteFile(tokenizer->GetCurToken().GetTagXML());
		if (tokenizer->GetNextToken().GetTokenName() == ".")
		{
			WriteFile(tokenizer->GetCurToken().GetTagXML());
			if (tokenizer->GetNextToken().GetType() == TYPE::T_IDENTIFIER)
			{
				WriteFile(tokenizer->GetCurToken().GetTagXML());
				if (tokenizer->GetNextToken().GetTokenName() == "(")
				{
					WriteFile(tokenizer->GetCurToken().GetTagXML());
					CompileExpressionList();
					if (tokenizer->GetCurToken().GetTokenName() == ")")
					{
						WriteFile(tokenizer->GetCurToken().GetTagXML());
						WriteFile(tokenizer->GetNextToken().GetTagXML());
						tokenizer->GetNextToken();
					}
				}
			}
		}
		else if(tokenizer->GetCurToken().GetTokenName() == "(")
		{
			WriteFile(tokenizer->GetCurToken().GetTagXML());
			CompileExpressionList();
			if (tokenizer->GetCurToken().GetTokenName() == ")")
			{
				WriteFile(tokenizer->GetCurToken().GetTagXML());
				WriteFile(tokenizer->GetNextToken().GetTagXML());
				tokenizer->GetNextToken();
			}
		}
	}
	WriteFile("</doStatement>");
}

void Sintaxer::CompileLet()
{
	WriteFile("<letStatement>");
	WriteFile(tokenizer->GetCurToken().GetTagXML());
	if (tokenizer->GetNextToken().GetType() == TYPE::T_IDENTIFIER)
	{
		WriteFile(tokenizer->GetCurToken().GetTagXML());
		tokenizer->GetNextToken();
		if (tokenizer->GetCurToken().GetTokenName() == "=")
		{
			WriteFile(tokenizer->GetCurToken().GetTagXML());
			CompileExpression();
			//tokenizer->GetCurToken().dump();
			if (tokenizer->GetNextToken().GetTokenName() == ";")
			{
				WriteFile(tokenizer->GetCurToken().GetTagXML());
				tokenizer->GetNextToken();
			}
		}
	}
	WriteFile("</letStatement>");	
}

void Sintaxer::CompileReturn()
{
	//printf("compile return\n");
	WriteFile("<returnStatement>");
	WriteFile(tokenizer->GetCurToken().GetKeyword().GetTagXML());
	//tokenizer->GetNextToken();
	if (tokenizer->GetCurToken().GetType() == TYPE::T_KEYWORDS) {
		CompileExpression();
	}
	else {
		WriteFile(tokenizer->GetNextToken().GetTagXML());
	}
	WriteFile("</returnStatement>");
		
}

void Sintaxer::CompileIf()
{
	WriteFile("<ifStatement>");
	WriteFile(tokenizer->GetCurToken().GetTagXML());
	if (tokenizer->GetNextToken().GetTokenName() == "(") {
		WriteFile(tokenizer->GetCurToken().GetTagXML());
		CompileExpression();
		if (tokenizer->GetCurToken().GetTokenName() == ")") {
			WriteFile(tokenizer->GetCurToken().GetTagXML());
			if (tokenizer->GetNextToken().GetTokenName() == "{")
			{
				WriteFile(tokenizer->GetCurToken().GetTagXML());
				tokenizer->GetNextToken();
				CompileStatements();
				if (tokenizer->GetCurToken().GetTokenName() == "}")
				{
					WriteFile(tokenizer->GetCurToken().GetTagXML());
					tokenizer->GetNextToken();
				}
			}
		}
	}
	WriteFile("</ifStatement>");

}

void Sintaxer::CompileWhile()
{
	WriteFile("<whileStatement>");
	WriteFile(tokenizer->GetCurToken().GetTagXML());
	if (tokenizer->GetNextToken().GetTokenName() == "(") {
		WriteFile(tokenizer->GetCurToken().GetTagXML());
		CompileExpression();
		if (tokenizer->GetCurToken().GetTokenName() == ")") {
			WriteFile(tokenizer->GetCurToken().GetTagXML());
			if (tokenizer->GetNextToken().GetTokenName() == "{")
			{
				WriteFile(tokenizer->GetCurToken().GetTagXML());
				tokenizer->GetNextToken();
				CompileStatements();
				if (tokenizer->GetCurToken().GetTokenName() == "}")
				{
					WriteFile(tokenizer->GetCurToken().GetTagXML());
					tokenizer->GetNextToken();
				}
			}
		}
	}
	WriteFile("</whileStatement>");
}


void Sintaxer::CompileExpression()
{

	while (tokenizer->GetNextToken().IsTerm()) {
		WriteFile("<expression>");
		CompileTerm();
		WriteFile("</expression>");
		//tokenizer->GetCurToken().dump();
		if (tokenizer->GetCurToken().GetTokenName() == ",")
		{
			WriteFile(tokenizer->GetCurToken().GetTagXML());
		}
		else if (tokenizer->GetCurToken().GetTokenName() == ";") {
			WriteFile(tokenizer->GetCurToken().GetTagXML());
			return;
		}
		else {
			return;
		}
	}
	if (tokenizer->GetCurToken().GetTokenName() == ";") {
		WriteFile(tokenizer->GetCurToken().GetTagXML());
	}
}

void Sintaxer::CompileTerm()
{
	//integerConstant | stringConstant | keywordConstant | varName | varName '[' expression ']' | subroutineCall | '(' expression ')' | unaryOp term
	if (tokenizer->GetCurToken().IsTerm()) {
		while (tokenizer->GetCurToken().IsTerm())
		{
			WriteFile("<term>");
			WriteFile(tokenizer->GetCurToken().GetTagXML());
			
			tokenizer->GetNextToken();
			if (tokenizer->GetCurToken().GetTokenName() == ".")
			{
				WriteFile(tokenizer->GetCurToken().GetTagXML());
				if (tokenizer->GetNextToken().GetType() == TYPE::T_IDENTIFIER)
				{
					WriteFile(tokenizer->GetCurToken().GetTagXML());

					if (tokenizer->GetNextToken().GetTokenName() == "(") {
						WriteFile(tokenizer->GetCurToken().GetTagXML());
						CompileExpressionList();
						if (tokenizer->GetCurToken().GetTokenName() == ";" || tokenizer->GetCurToken().GetTokenName() == ")")
						{
							WriteFile(tokenizer->GetCurToken().GetTagXML());
						}
					}
				}
			}

			else if (tokenizer->GetCurToken().GetTokenName() == "(")
			{
				WriteFile("<term>");
				WriteFile(tokenizer->GetCurToken().GetTagXML());
				CompileExpression();
				WriteFile(tokenizer->GetCurToken().GetTagXML());
				WriteFile("</term>");
				tokenizer->GetNextToken();
			}
			else if(tokenizer->GetCurToken().GetType() == TYPE::T_IDENTIFIER){
				WriteFile("<term>");
				WriteFile(tokenizer->GetCurToken().GetTagXML());
				tokenizer->GetNextToken();
				WriteFile("</term>");
			}
			
			if (tokenizer->GetCurToken().GetTokenName() == "=") {
				WriteFile("</term>");
				WriteFile(tokenizer->GetCurToken().GetTagXML());
				tokenizer->GetNextToken();
			}

			
		}
		WriteFile("</term>");
	}
	else {
		printf("nao eh termo %s\n", tokenizer->GetCurToken().GetTokenName().c_str());
	}
}

void Sintaxer::CompileExpressionList()
{
	WriteFile("<expressionList>");
	CompileExpression();
	WriteFile("</expressionList>");

}

void Sintaxer::WriteFile(std::string str)
{
	fprintf(m_output, "%s\n", str.c_str());
	printf("%s\n", str.c_str());
	count++;
}
