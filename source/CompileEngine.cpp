#include "stdafx.h"
#include "CompileEngine.h"
#include "utils.h"

//int count = 0;
CompilationEngine::CompilationEngine():
	m_output(NULL),
	tokenizer(NULL),
	m_spaces(0)
{
}

CompilationEngine::CompilationEngine(std::string filename):
	m_spaces(0),
	m_filename(filename)
{
	m_output = fopen(Utils::GetXmlFileName(filename).c_str(), "w");
	tokenizer = new Tokenizer(filename.c_str());
	
	/*while(tokenizer->GetNextToken().IsValid())
		if(tokenizer->GetCurToken().GetKeyword().Get() == KWDTP::KCLASS)
			CompileClass();

	fclose(m_output);*/
	Compile();
}

void CompilationEngine::Compile()
{
	if (!m_output) {
		printf("nao pode criar o arquivo (%s)\n", Utils::GetXmlFileName(m_filename).c_str());
		return;
	}
	while (tokenizer->GetNextToken().IsValid()) {
		if (tokenizer->GetCurToken().GetKeyword().Get() == KWDTP::KCLASS) {
			CompileClass();
		}
	}
	fclose(m_output);
}

CompilationEngine::~CompilationEngine()
{
	fclose(m_output);
	delete tokenizer;
}

void CompilationEngine::CompileClass()
{
	const char* classTemplate[] = { "class", "className" ,"{", "classVarDec", "subroutineDec","}", NULL };

	openXMLTag("<class>");
	if (tokenizer->GetCurToken().GetKeyword().Get() == KWDTP::KCLASS)
	{
		WriteFile(tokenizer->GetCurToken().GetTagXML());
		tokenizer->GetNextToken();

		if (tokenizer->GetType() == TYPE::T_IDENTIFIER)
		{
			WriteFile(tokenizer->GetCurToken().GetTagXML());
			tokenizer->GetNextToken();
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
	
	closeXMLTag("</class>");
}

void CompilationEngine::CompileClassVarDec()
{
	//( 'static' | 'field' ) type varName ( ',' varName)* ';'
	
	while (tokenizer->GetNextToken().GetKeyword().IsClassVarDec())
	{
		openXMLTag("<classVarDec>");
		WriteFile(tokenizer->GetCurToken().GetTagXML());

		if (tokenizer->GetNextToken().GetKeyword().IsVarType() || tokenizer->GetCurToken().GetType() == TYPE::T_IDENTIFIER) {
			WriteFile(tokenizer->GetCurToken().GetTagXML());
			if (tokenizer->GetNextToken().GetType() == TYPE::T_IDENTIFIER) {
				WriteFile(tokenizer->GetCurToken().GetTagXML());
				if (tokenizer->GetNextToken().GetType() == TYPE::T_SYMBOLS) {
					if (tokenizer->GetCurToken().GetRealName() == ";") {
						WriteFile(tokenizer->GetCurToken().GetTagXML());
					}
					else {
						while ((tokenizer->GetCurToken().GetRealName() == ",")) {
							WriteFile(tokenizer->GetCurToken().GetTagXML());
							if (tokenizer->GetNextToken().GetType() == TYPE::T_IDENTIFIER) {
								WriteFile(tokenizer->GetCurToken().GetTagXML());
								if (tokenizer->GetNextToken().GetRealName() == ";") {
									WriteFile(tokenizer->GetCurToken().GetTagXML());
								}
							}
						}
					}
				}
			}
			else {
				printf("esperado um identificador\n");
				return;
			}
		}
		closeXMLTag("</classVarDec>");
	}
}

void CompilationEngine::CompileSubroutine()
{	
	"('constructor' | 'function' | 'method') ('void' | type) subroutineName '(' parameterList ')' subroutineBody";


	while (tokenizer->GetCurToken().GetKeyword().IsSubroutine())
	{
		openXMLTag("<subroutineDec>");
		if (tokenizer->GetCurToken().GetKeyword().Get() == KWDTP::KCONSTRUCTOR)
		{
			WriteFile(tokenizer->GetCurToken().GetTagXML());
			if (tokenizer->GetNextToken().GetType() == TYPE::T_IDENTIFIER)
			{
				WriteFile(tokenizer->GetCurToken().GetTagXML());
				CompileSubroutineParameters();
			}
		}
		else if (tokenizer->GetCurToken().GetKeyword().Get() == KWDTP::KFUNCTION)
		{
			WriteFile(tokenizer->GetCurToken().GetTagXML());
			if (tokenizer->GetNextToken().GetKeyword().Get() == KWDTP::KVOID)
			{
				WriteFile(tokenizer->GetCurToken().GetTagXML());
				CompileSubroutineParameters();
			}
		}
		else if (tokenizer->GetCurToken().GetKeyword().Get() == KWDTP::KMETHOD)
		{
			WriteFile(tokenizer->GetCurToken().GetTagXML());
			if (tokenizer->GetNextToken().GetKeyword().Get() == KWDTP::KVOID)
			{
				WriteFile(tokenizer->GetCurToken().GetTagXML());
				CompileSubroutineParameters();
			}
		}
		closeXMLTag("</subroutineDec>");
		tokenizer->GetNextToken();
	}
	
}

void CompilationEngine::CompileSubroutineParameters()
{
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

void CompilationEngine::CompileSubroutineBody()
{
	tokenizer->GetNextToken();
	openXMLTag("<subroutineBody>");
	if (tokenizer->GetCurToken().GetTokenName() == "{")
	{
		WriteFile(tokenizer->GetCurToken().GetTagXML());
		tokenizer->GetNextToken();
		while (tokenizer->GetCurToken().IsValid())
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
	closeXMLTag("</subroutineBody>");
}

void CompilationEngine::CompileParameterList()
{
	//tokenizer->GetNextToken();
	openXMLTag("<parameterList>");
	while (tokenizer->GetNextToken().IsValid() && tokenizer->GetCurToken().GetKeyword().IsVarType() && tokenizer->GetCurToken().GetTokenName() != ")")
	{
		WriteFile(tokenizer->GetCurToken().GetTagXML());
		if (tokenizer->GetNextToken().GetType() == TYPE::T_IDENTIFIER) {
			WriteFile(tokenizer->GetCurToken().GetTagXML());
			if (tokenizer->GetNextToken().GetTokenName() == ",")
			{
				WriteFile(tokenizer->GetCurToken().GetTagXML());
			}
			else if(tokenizer->GetCurToken().GetTokenName() == ")") {
				break;
			}
		}
	}
	closeXMLTag("</parameterList>");
}

void CompilationEngine::CompileVarDec()
{
	while (tokenizer->GetCurToken().GetKeyword().Get() == KWDTP::KVAR)
	{
		openXMLTag("<varDec>");
		WriteFile(tokenizer->GetCurToken().GetTagXML());
		if (tokenizer->GetNextToken().GetKeyword().IsVarType() || tokenizer->GetCurToken().GetType() == TYPE::T_IDENTIFIER) {
			WriteFile(tokenizer->GetCurToken().GetTagXML());
			while (tokenizer->GetNextToken().GetType() == TYPE::T_IDENTIFIER)
			{
				WriteFile(tokenizer->GetCurToken().GetTagXML());
				if (tokenizer->GetNextToken().GetTokenName() == ";") {
					WriteFile(tokenizer->GetCurToken().GetTagXML());
				}
				else if (tokenizer->GetCurToken().GetTokenName() == ",") {
					WriteFile(tokenizer->GetCurToken().GetTagXML());
				}
			}
		}
		closeXMLTag("</varDec>");
	}
}
void CompilationEngine::CompileStatements()
{
	openXMLTag("<statements>");
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
	}
	closeXMLTag("</statements>");
}

void CompilationEngine::CompileSubroutineCall()
{
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
void CompilationEngine::CompileDo()
{
	openXMLTag("<doStatement>");
	WriteFile(tokenizer->GetCurToken().GetTagXML());
	if (tokenizer->GetNextToken().GetType() == TYPE::T_IDENTIFIER)
	{
		WriteFile(tokenizer->GetCurToken().GetTagXML());
		if (tokenizer->GetNextToken().GetTokenName() == ".")
		{
			WriteFile(tokenizer->GetCurToken().GetTagXML());
			CompileSubroutineCall();
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
	closeXMLTag("</doStatement>");
}

void CompilationEngine::CompileLet()
{
	openXMLTag("<letStatement>");
	WriteFile(tokenizer->GetCurToken().GetTagXML());
	if (tokenizer->GetNextToken().GetType() == TYPE::T_IDENTIFIER)
	{
		WriteFile(tokenizer->GetCurToken().GetTagXML());
		tokenizer->GetNextToken();
		if (tokenizer->GetCurToken().GetTokenName() == "=")
		{
			CompileLetAssignments();
		}
		else if(tokenizer->GetCurToken().GetTokenName() == "[")
		{
			WriteFile(tokenizer->GetCurToken().GetTagXML());
			CompileExpression();
			if (tokenizer->GetCurToken().GetTokenName() == "]")
			{
				WriteFile(tokenizer->GetCurToken().GetTagXML());
				tokenizer->GetNextToken();
				if (tokenizer->GetCurToken().GetTokenName() == "=")
				{
					CompileLetAssignments();
				}
			}
		}
	}
	closeXMLTag("</letStatement>");	
}

void CompilationEngine::CompileLetAssignments()
{
	WriteFile(tokenizer->GetCurToken().GetTagXML());
	CompileExpression();
	if (tokenizer->GetNextToken().GetTokenName() == ";")
	{
		WriteFile(tokenizer->GetCurToken().GetTagXML());
		tokenizer->GetNextToken();
	}
}

void CompilationEngine::CompileReturn()
{
	openXMLTag("<returnStatement>");
	WriteFile(tokenizer->GetCurToken().GetKeyword().GetTagXML());
	if (tokenizer->GetCurToken().GetType() == TYPE::T_KEYWORDS) {
		CompileExpression();
	}
	else {
		WriteFile(tokenizer->GetNextToken().GetTagXML());
	}
	closeXMLTag("</returnStatement>");
		
}

void CompilationEngine::CompileIf()
{
	openXMLTag("<ifStatement>");
	WriteFile(tokenizer->GetCurToken().GetTagXML());
	if (tokenizer->GetNextToken().GetTokenName() == "(") {
		WriteFile(tokenizer->GetCurToken().GetTagXML());
		CompileExpression();
	
		if (tokenizer->GetCurToken().GetTokenName() == ")") {
			WriteFile(tokenizer->GetCurToken().GetTagXML());
			tokenizer->GetNextToken();
			while (tokenizer->GetCurToken().GetTokenName() == "{")
			{
				WriteFile(tokenizer->GetCurToken().GetTagXML());
				tokenizer->GetNextToken();
				CompileStatements();
				if (tokenizer->GetCurToken().GetTokenName() == "}")
				{
					WriteFile(tokenizer->GetCurToken().GetTagXML());
					tokenizer->GetNextToken();
					if (tokenizer->GetCurToken().GetKeyword().Get() == KWDTP::KELSE)
					{
						WriteFile(tokenizer->GetCurToken().GetTagXML());
						if (tokenizer->GetNextToken().GetKeyword().Get() == KWDTP::KIF) {
							CompileIf();
						}
					}
				}
			}
		}
	}
	closeXMLTag("</ifStatement>");

}

void CompilationEngine::CompileWhile()
{
	openXMLTag("<whileStatement>");
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
	closeXMLTag("</whileStatement>");
}


void CompilationEngine::CompileExpression()
{
	while (tokenizer->GetNextToken().IsTerm()) {
		openXMLTag("<expression>");
		CompileTerm();
		closeXMLTag("</expression>");

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

void CompilationEngine::CompileTerm()
{
	//integerConstant | stringConstant | keywordConstant | varName | varName '[' expression ']' | subroutineCall | '(' expression ')' | unaryOp term
	if (tokenizer->GetCurToken().IsTerm()) {
		while (tokenizer->GetCurToken().IsTerm())
		{
			openXMLTag("<term>");
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
							closeXMLTag("</term>");
						}
					}
				}
			}
			else if (tokenizer->GetCurToken().IsOp()) {
				closeXMLTag("</term>");
				WriteFile(tokenizer->GetCurToken().GetTagXML());
				tokenizer->GetNextToken();
			}
			else if (tokenizer->GetCurToken().GetTokenName() == "[") {
				WriteFile(tokenizer->GetCurToken().GetTagXML());
				CompileExpression();
				if (tokenizer->GetCurToken().GetTokenName() == "]")
				{
					WriteFile(tokenizer->GetCurToken().GetTagXML());
					closeXMLTag("</term>" );
				}
			}

			else if (tokenizer->GetCurToken().GetTokenName() == "(" )
			{
				openXMLTag("<term>");
				WriteFile(tokenizer->GetCurToken().GetTagXML());
				CompileExpression();
				WriteFile(tokenizer->GetCurToken().GetTagXML());
				closeXMLTag("</term>");
				if(tokenizer->GetCurToken().GetTokenName() == ")")
					closeXMLTag("</term>");

				tokenizer->GetNextToken();
			}
			else if(tokenizer->GetCurToken().GetType() == TYPE::T_IDENTIFIER){
				CompileTerm();
				closeXMLTag("</term>");
			}
		
			else if (tokenizer->GetCurToken().GetTokenName() == "=") {
				CompileTerm();
			}
			else 
				closeXMLTag("</term>");
		}
	}
	else {
		printf("nao eh termo %s\n", tokenizer->GetCurToken().GetTokenName().c_str());
	}
}



void CompilationEngine::CompileExpressionList()
{
	openXMLTag("<expressionList>");
	CompileExpression();
	closeXMLTag("</expressionList>");
	
}

void CompilationEngine::WriteFile(std::string str)
{
	fprintf(m_output, "%s%s\n", std::string(m_spaces * 2, ' ').c_str() , str.c_str());
	printf("%s%s\n", std::string(m_spaces * 2, ' ').c_str() , str.c_str());
}

void CompilationEngine::openXMLTag(std::string str)
{
	WriteFile(str);
	m_spaces++;
}
void CompilationEngine::closeXMLTag(std::string str)
{
	m_spaces--;
	WriteFile(str);
}