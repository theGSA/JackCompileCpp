#include "stdafx.h"
#include "CompileEngine.h"
#include "utils.h"

//int count = 0;
CompileEngine::CompileEngine():
	m_output(NULL),
	tokenizer(NULL),
	m_spaces(0)
{
}

CompileEngine::CompileEngine(std::string filename):
	m_spaces(0),
	m_filename(filename)
{
	m_output = fopen(Utils::GetXmlFileName(filename).c_str(), "w");
	tokenizer = new Tokenizer(filename.c_str());
	//if (m_output)
		//fclose(m_output);

	Compile();
}

void CompileEngine::Compile()
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

CompileEngine::~CompileEngine()
{
	fclose(m_output);
	delete tokenizer;
}

void CompileEngine::CompileClass()
{
	///{ "class", "className" ,"{", "classVarDec", "subroutineDec","}"

	openXMLTag("<class>");
	Eat("class");
	tokenizer->GetNextToken();
	Eat("identifier");
	tokenizer->GetNextToken();
	Eat("{");
	CompileClassVarDec();
	CompileSubroutine();
	Eat("}");
	closeXMLTag("</class>");
}

void CompileEngine::CompileClassVarDec()
{
	//( 'static' | 'field' ) type varName ( ',' varName)* ';'
	bool in = false;
	while (tokenizer->GetNextToken().GetKeyword().IsClassVarDec()) {
		in = true;
		openXMLTag("<classVarDec>");
		switch (tokenizer->GetCurToken().GetKeyword().Get())
		{
		case KWDTP::KSTATIC:
			Eat("static");
			break;
		case KWDTP::KFIELD:
			Eat("field");
			break;
		default:
			break;
		}
		switch (tokenizer->GetNextToken().GetType())
		{
			case TYPE::T_KEYWORDS:
				Eat("varname");
				tokenizer->GetNextToken();
				Eat("identifier");
				break;
			case TYPE::T_IDENTIFIER:
				Eat("identifier");
				tokenizer->GetNextToken();
				Eat("identifier");
				break;
			default:
				Eat("");
				break;
		}

		while (tokenizer->GetNextToken().GetTokenName() == ",")
		{
			Eat(",");
			tokenizer->GetNextToken();
			Eat("identifier");
		}
		Eat(";");
		closeXMLTag("</classVarDec>");
	}
	if (!in)
		Eat("class var(static ou field)");
}

void CompileEngine::CompileSubroutine()
{	
	//"('constructor' | 'function' | 'method') ('void' | type) subroutineName '(' parameterList ')' subroutineBody";

	bool in = false;
	while (tokenizer->GetCurToken().GetKeyword().IsSubroutine())
	{
		in = true;
		openXMLTag("<subroutineDec>");
		if (tokenizer->GetCurToken().GetKeyword().Get() == KWDTP::KCONSTRUCTOR)
		{
			Eat("constructor");
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
		else {
			Eat("constructor' | 'function' | 'method') ('void' | type) subroutineName '(' parameterList ')' subroutineBody");
		}
		closeXMLTag("</subroutineDec>");
		tokenizer->GetNextToken();
	}

	if (!in)
		Eat("constructor' | 'function' | 'method') ('void' | type) subroutineName '(' parameterList ')' subroutineBody");
	//closeXMLTag("</subroutineDec>");
			
}

void CompileEngine::CompileSubroutineParameters()
{
	tokenizer->GetNextToken();
	Eat("identifier");
	tokenizer->GetNextToken();
	Eat("(");
	CompileParameterList();
	Eat(")");
	CompileSubroutineBody();
	
}

void CompileEngine::CompileSubroutineBody()
{
	tokenizer->GetNextToken();
	openXMLTag("<subroutineBody>");
	Eat("{");

	tokenizer->GetNextToken();
	while (tokenizer->GetCurToken().IsValid())
	{
		if (tokenizer->GetCurToken().GetKeyword().IsStatement())
			CompileStatements();
		else if (tokenizer->GetCurToken().GetKeyword().Get() == KWDTP::KVAR)
			CompileVarDec();
		else if (tokenizer->GetCurToken().GetTokenName() == "}") {
			WriteFile(tokenizer->GetCurToken().GetTagXML());
			break;
		}
	}

	closeXMLTag("</subroutineBody>");
}

void CompileEngine::CompileParameterList()
{

	openXMLTag("<parameterList>");
	while (tokenizer->GetNextToken().GetKeyword().IsVarType())
	{
		Eat("varname");
		tokenizer->GetNextToken().GetType();
		Eat("identifier");
		if (tokenizer->GetNextToken().GetTokenName() == ","){
			WriteFile(tokenizer->GetCurToken().GetTagXML());
		}
		else if (tokenizer->GetCurToken().GetTokenName() == ")")
			break;
		else
			Eat("' , ou ) '");
		
	}
	closeXMLTag("</parameterList>");
}

void CompileEngine::CompileVarDec()
{
		//'var' type varName ( ',' varName)* ';'
		openXMLTag("<varDec>");
		Eat("var");
		tokenizer->GetNextToken();
		//Eat("varname");
		switch (tokenizer->GetCurToken().GetType())
		{
		case TYPE::T_IDENTIFIER:
			Eat("identifier");
			break;
		case TYPE::T_KEYWORDS:
			Eat("varname");
			break;
		default:
			Eat("Identifier | varType");
			break;
		}
		tokenizer->GetNextToken();
		Eat("identifier");
		tokenizer->GetNextToken();
		while (tokenizer->GetCurToken().GetTokenName() == ",")
		{
			Eat(",");
			tokenizer->GetNextToken();
			Eat("identifier");
			tokenizer->GetNextToken();
		}
		Eat(";");
		tokenizer->GetNextToken();
		closeXMLTag("</varDec>");
}
void CompileEngine::CompileStatements()
{
	//tem que rever
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

void CompileEngine::CompileSubroutineCall()
{
	switch (tokenizer->GetCurToken().GetType())
	{
	case TYPE::T_SYMBOLS:
		if (tokenizer->GetCurToken().GetTokenName() == ".") {
			Eat(".");
			tokenizer->GetNextToken();
			Eat("identifier");
			tokenizer->GetNextToken();
			Eat("(");
			tokenizer->GetNextToken();
			CompileExpressionList();
			Eat(")");
		}
		else if (tokenizer->GetCurToken().GetTokenName() == "(")
		{
			Eat("(");
			tokenizer->GetNextToken();
			CompileExpressionList();
			Eat(")");
		}
		break;
	default:
		Eat("SYMBOL");
		break;
	}
	tokenizer->GetNextToken();
}
void CompileEngine::CompileDo()
{
	//'do' subroutineCall ';'
	openXMLTag("<doStatement>");
	Eat("do");
	tokenizer->GetNextToken();
	Eat("identifier");
	tokenizer->GetNextToken();
	CompileSubroutineCall();
	Eat(";");
	tokenizer->GetNextToken();
	closeXMLTag("</doStatement>");
}

void CompileEngine::CompileLet()
{
	openXMLTag("<letStatement>");
	Eat("let");

	tokenizer->GetNextToken();
	switch (tokenizer->GetCurToken().GetType())
	{
	case TYPE::T_IDENTIFIER:
		Eat(tokenizer->GetCurToken().GetTokenName());
		tokenizer->GetNextToken();
		if (tokenizer->GetCurToken().GetTokenName() == "=")
		{
			Eat("=");
			tokenizer->GetNextToken();
			CompileExpression();
			if (tokenizer->GetCurToken().GetTokenName() == ";")
				Eat(";");
			tokenizer->GetNextToken();
		}
		else if (tokenizer->GetCurToken().GetTokenName() == "[")
		{
			Eat("[");
			tokenizer->GetNextToken();
			CompileExpression();
			Eat("]");
			tokenizer->GetNextToken();
			if (tokenizer->GetCurToken().GetTokenName() == "=")
			{
				Eat("=");
				tokenizer->GetNextToken();
				CompileExpression();
				if (tokenizer->GetCurToken().GetTokenName() == ";")
					Eat(";");
				tokenizer->GetNextToken();
			}
		}
		break;
	}

	closeXMLTag("</letStatement>");
}

void CompileEngine::CompileLetAssignments()
{
	

}

void CompileEngine::CompileReturn()
{
	openXMLTag("<returnStatement>");
	Eat("return");
	tokenizer->GetNextToken();

	if (tokenizer->GetCurToken().GetType() == TYPE::T_KEYWORDS ||
		tokenizer->GetCurToken().GetType() == TYPE::T_IDENTIFIER ) {
		CompileExpression();
	}

	Eat(";");
	tokenizer->GetNextToken();
	closeXMLTag("</returnStatement>");
		
}

void CompileEngine::CompileIf()
{
	
	openXMLTag("<ifStatement>");
	Eat("if");
	tokenizer->GetNextToken();
	Eat("(");
	tokenizer->GetNextToken();
	CompileExpression();
	Eat(")");
	tokenizer->GetNextToken();
	Eat("{");
	tokenizer->GetNextToken();
	CompileStatements();
	Eat("}");
	tokenizer->GetNextToken();
	switch (tokenizer->GetCurToken().GetKeyword().Get())
	{
	case KWDTP::KELSE:
		Eat("else");
		tokenizer->GetNextToken();
		switch (tokenizer->GetCurToken().GetKeyword().Get())
		{
		case KWDTP::KIF:
			CompileIf();
			break;
		default:
			Eat("{");
			tokenizer->GetNextToken();
			CompileStatements();
			Eat("}");
			tokenizer->GetNextToken();
			break;
		}
		break;
	default:
		break;
	}
	closeXMLTag("</ifStatement>");
}

void CompileEngine::CompileWhile()
{
	//'while' '(' expression ')' '{' statements '}'
	openXMLTag("<whileStatement>");
	Eat("while");
	tokenizer->GetNextToken();
	Eat("(");
	tokenizer->GetNextToken();
	CompileExpression();
	Eat(")");
	tokenizer->GetNextToken();
	Eat("{");
	tokenizer->GetNextToken();
	CompileStatements();
	Eat("}");
	tokenizer->GetNextToken();

	closeXMLTag("</whileStatement>");
}


void CompileEngine::CompileExpression()
{

	openXMLTag("<expression>");
	CompileTerm();
	while (tokenizer->GetCurToken().IsOp())
	{
		Eat(tokenizer->GetCurToken().GetTokenName());
		tokenizer->GetNextToken();
		CompileTerm();
	}
	closeXMLTag("</expression>");
}

void CompileEngine::CompileTerm()
{
	openXMLTag("<term>");
	switch (tokenizer->GetCurToken().GetType())
	{
		case TYPE::T_IDENTIFIER:
			Eat("identifier");
			if (tokenizer->GetPeekToken().GetTokenName() == ".")
			{
				tokenizer->GetNextToken();
				CompileSubroutineCall();
				closeXMLTag("</term>");
				return;
			}
			else if (tokenizer->GetPeekToken().GetTokenName() == "[")
			{
				tokenizer->GetNextToken();
				Eat("[");
				tokenizer->GetNextToken();
				CompileExpression();
				Eat("]");
			}
		break;
		case TYPE::T_SYMBOLS:
			if (tokenizer->GetCurToken().GetTokenName() == "(") {
				Eat("(");
				tokenizer->GetNextToken();
				CompileExpression();
				Eat(")");
			}
			else if(tokenizer->GetCurToken().IsOpUnary())
			{
				Eat(tokenizer->GetCurToken().GetTokenName());
				tokenizer->GetNextToken();
				CompileTerm();
				closeXMLTag("</term>");
				return;
			}
		break;
		
		case TYPE::T_KEYWORDS:
			if (tokenizer->GetCurToken().GetKeyword().IsKeywordConstant())
			{
				Eat(tokenizer->GetCurToken().GetTokenName());
			}
			break;
		case TYPE::T_INT_CONST:
			Eat(tokenizer->GetCurToken().GetTokenName());
			break;
		case TYPE::T_STRING_CONST:
			Eat(tokenizer->GetCurToken().GetTokenName());
			break;
	default:
		break;
	}
	tokenizer->GetNextToken();
	closeXMLTag("</term>");
}

void CompileEngine::CompileExpressionList()
{

	openXMLTag("<expressionList>");
	if (tokenizer->GetCurToken().GetType() != TYPE::T_SYMBOLS || tokenizer->GetCurToken().GetTokenName() == "(")
		CompileExpression();

	while (tokenizer->GetCurToken().GetTokenName() == ",")
	{
		Eat(",");
		tokenizer->GetNextToken();
		CompileExpression();
	}
	closeXMLTag("</expressionList>");
}

void CompileEngine::Eat(std::string str)
{
	if (tokenizer->GetCurToken().GetTokenName() == str ||
		tokenizer->GetCurToken().GetTypeName() == str ||
		(tokenizer->GetCurToken().GetKeyword().IsVarType() && str == "varname"))
	{
		WriteFile(tokenizer->GetCurToken().GetTagXML());
	}
	else {
		printf("esperado %s mas recebeu %s: L: %d ; C: %d\n", str.c_str(), tokenizer->GetCurToken().GetRealName().c_str(), tokenizer->GetCurToken().m_line, tokenizer->GetCurToken().m_collumn);
		exit(0);
	}
}
void CompileEngine::WriteFile(std::string str)
{
	//m_output = fopen(Utils::GetXmlFileName(m_filename).c_str(), "a+");
	fprintf(m_output, "%s%s\n", std::string(m_spaces * 2, ' ').c_str() , str.c_str());
	printf("%s%s\n", std::string(m_spaces * 2, ' ').c_str() , str.c_str());
	//fclose(m_output);
}

void CompileEngine::openXMLTag(std::string str)
{
	WriteFile(str);
	m_spaces++;
}
void CompileEngine::closeXMLTag(std::string str)
{
	m_spaces--;
	WriteFile(str);
}