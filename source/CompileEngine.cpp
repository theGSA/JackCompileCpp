#include "stdafx.h"
#include "CompileEngine.h"
#include "utils.h"


CompileEngine::CompileEngine():
m_output(NULL),
tokenizer(NULL),
m_spaces(0),
sytb(NULL),
curSubRoutine(),
vm(NULL),
m_arg(0),
m_ifCount(0),
m_whileCount(0)
{
}

CompileEngine::CompileEngine(std::string filename):
	m_spaces(0),
	m_filename(filename),
	m_arg(0)
{
	m_output = fopen(Utils::GetXmlFileName(filename).c_str(), "w");
	if (!m_output) {
		printf("nao foi possivel criar %s\n", Utils::GetXmlFileName(filename).c_str());
		return;
	}
	else
	{
		printf("%s criado com sucesso\n", Utils::GetXmlFileName(filename).c_str());
	}
	tokenizer = new Tokenizer(filename.c_str());
	vm = new VMWriter(Utils::GetVMFileName(filename).c_str());
	sytb = new SymbolTable();
	Compile();
}

void CompileEngine::Compile()
{
	if (!m_output) {
		printf("nao pode criar o arquivo (%s)\n", Utils::GetXmlFileName(m_filename).c_str());
		exit(0);
	}
	while (tokenizer->GetNextToken().IsValid()) {
		if (tokenizer->GetCurToken().GetKeyword().Get() == KWDTP::KCLASS) {
			CompileClass();
		}
	}
	fclose(m_output);
	vm->close();
}

CompileEngine::~CompileEngine()
{
	fclose(m_output);
	delete tokenizer;
	delete vm;
	delete sytb;
}

void CompileEngine::CompileClass()
{
	///{ "class", "className" ,"{", "classVarDec", "subroutineDec","}"

	openXMLTag("<class>");
	Eat("class");
	tokenizer->GetNextToken();
	Eat("identifier");
	className = tokenizer->GetCurToken().GetTokenName();
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
	std::string  name, type;
	Symbol kind = Symbol::NONE;

	while (tokenizer->GetNextToken().GetKeyword().IsClassVarDec()) {
		in = true;
		openXMLTag("<classVarDec>");
		switch (tokenizer->GetCurToken().GetKeyword().Get())
		{
		case KWDTP::KSTATIC:
			Eat("static");
			kind = Symbol::STATIC;
			break;
		case KWDTP::KFIELD:
			Eat("field");
			kind = Symbol::FIELD;
			break;
		default:
			break;
		}
		switch (tokenizer->GetNextToken().GetType())
		{
			case TYPE::T_KEYWORDS:
				Eat("varname");
				type = tokenizer->GetCurToken().GetTokenName();
				tokenizer->GetNextToken();
				Eat("identifier");
				name = tokenizer->GetCurToken().GetTokenName();
				break;
			case TYPE::T_IDENTIFIER:
				Eat("identifier");
				type = tokenizer->GetCurToken().GetTokenName();
				tokenizer->GetNextToken();
				Eat("identifier");
				name  = tokenizer->GetCurToken().GetTokenName();
				break;
			default:
				Eat("");
				break;
		}
		sytb->define(name, type, kind);
		while (tokenizer->GetNextToken().GetTokenName() == ",")
		{
			Eat(",");
			tokenizer->GetNextToken();
			Eat("identifier");
			sytb->define(tokenizer->GetCurToken().GetTokenName(), type, kind);
		}
		Eat(";");
		closeXMLTag("</classVarDec>");
	}
	//if (!in)
		//Eat("class var(static ou field)");
}

void CompileEngine::CompileSubroutine()
{	
	//"('constructor' | 'function' | 'method') ('void' | type) subroutineName '(' parameterList ')' subroutineBody";

	bool in = false;
	while (tokenizer->GetCurToken().GetKeyword().IsSubroutine())
	{
		in = true;
		openXMLTag("<subroutineDec>");
		sytb->startSubroutine();
		m_ifCount = 0;
		m_whileCount = 0;

		curSubRoutine.type = tokenizer->GetCurToken().GetKeyword().Get();

		if (tokenizer->GetCurToken().GetKeyword().Get() == KWDTP::KCONSTRUCTOR)
		{
			Eat("constructor");

			if (tokenizer->GetNextToken().GetType() == TYPE::T_IDENTIFIER)
			{
				Eat("identifier");
				CompileSubroutineParameters();
			}
		}
		else if (tokenizer->GetCurToken().GetKeyword().Get() == KWDTP::KFUNCTION)
		{
			Eat("function");
			if (tokenizer->GetNextToken().GetKeyword().Get() == KWDTP::KVOID)
			{
				Eat("void");
				CompileSubroutineParameters();
			}
			else if (tokenizer->GetCurToken().GetType() == TYPE::T_IDENTIFIER)
			{
				Eat("identifier");
				CompileSubroutineParameters();
			}
		}
		else if (tokenizer->GetCurToken().GetKeyword().Get() == KWDTP::KMETHOD)
		{
			Eat("method");
			sytb->define("this", className, Symbol::ARG);
			tokenizer->GetNextToken();
			if (tokenizer->GetCurToken().GetKeyword().Get() == KWDTP::KVOID)
			{
				Eat("void");
				CompileSubroutineParameters();
			}
			else if (tokenizer->GetCurToken().GetKeyword().Get() == KWDTP::KINT)
			{
				Eat("int");
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
	curSubRoutine.name = className + "." + tokenizer->GetCurToken().GetTokenName();
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
	while (tokenizer->GetCurToken().GetKeyword().Get() == KWDTP::KVAR) {
		CompileVarDec();
	}
	vm->writeFunction(curSubRoutine.name, sytb->VarCount(Symbol::VAR));
	
	if (curSubRoutine.type == KWDTP::KCONSTRUCTOR) {
		vm->writePush(Segment::CONSTA, sytb->VarCount(Symbol::FIELD));
		vm->writeCall("Memory.alloc", 1);
		vm->writePop(Segment::POINTER, 0);
	}
	else if(curSubRoutine.type == KWDTP::KMETHOD){
		vm->writePush(Segment::ARG, 0);
		vm->writePop(Segment::POINTER, 0);
	}

	if (tokenizer->GetCurToken().GetKeyword().IsStatement())
		CompileStatements();
	Eat("}");
	closeXMLTag("</subroutineBody>");
}

void CompileEngine::CompileParameterList()
{

	openXMLTag("<parameterList>");
	Token curToken = tokenizer->GetNextToken();

	//m_arg = 0;
	while (curToken.GetKeyword().IsVarType() || curToken.GetType() == TYPE::T_IDENTIFIER)
	{
		std::string name, type;
		Symbol kind = Symbol::ARG;

		if (curToken.GetKeyword().IsVarType()) {
			Eat("varname");
		}
		else if (curToken.GetType() == TYPE::T_IDENTIFIER)
			Eat("identifier");

		type = curToken.GetTokenName();
		curToken = tokenizer->GetNextToken();
		Eat("identifier");
		name = curToken.GetTokenName();
		sytb->define(name, type, kind);

		curToken = tokenizer->GetNextToken();
		//m_arg++;
		if (curToken.GetTokenName() == ","){
			Eat(",");
		}
		else if (curToken.GetTokenName() == ")")
			break;
		else
			Eat("' , ou ) '");

		curToken = tokenizer->GetNextToken();
	}
	closeXMLTag("</parameterList>");

}

void CompileEngine::CompileVarDec()
{
		//'var' type varName ( ',' varName)* ';'
	std::string name, type;
	Symbol kind = Symbol::VAR;

	openXMLTag("<varDec>");
	Eat("var");
	tokenizer->GetNextToken();
	switch (tokenizer->GetCurToken().GetType())
	{
	case TYPE::T_IDENTIFIER:
		Eat("identifier");
		type = tokenizer->GetCurToken().GetTokenName();
		break;
	case TYPE::T_KEYWORDS:
		Eat("varname");
		type = tokenizer->GetCurToken().GetTokenName();
		break;
	default:
		Eat("Identifier | varType");
		break;
	}
	tokenizer->GetNextToken();
	Eat("identifier");
	name = tokenizer->GetCurToken().GetTokenName();
	sytb->define(name, type, kind);
	tokenizer->GetNextToken();
	while (tokenizer->GetCurToken().GetTokenName() == ",")
	{
		Eat(",");
		tokenizer->GetNextToken();
		Eat("identifier");
		name = tokenizer->GetCurToken().GetTokenName();
		tokenizer->GetNextToken();
		sytb->define(name, type, kind);
	}
	Eat(";");
	tokenizer->GetNextToken();
	closeXMLTag("</varDec>");
}
void CompileEngine::CompileStatements()
{
	//tem que rever
	openXMLTag("<statements>");
	//vm->writePush(Segment::ARG, 0);
	//vm->writePop(Segment::POINTER, 0);
	while (tokenizer->GetCurToken().GetKeyword().IsStatement()) {
		if (tokenizer->GetCurToken().GetKeyword().Get() == KWDTP::KLET)
			CompileLet();
		else if (tokenizer->GetCurToken().GetKeyword().Get() == KWDTP::KIF) {
			CompileIf();
		}
		else if (tokenizer->GetCurToken().GetKeyword().Get() == KWDTP::KWHILE) {
			CompileWhile();
		}
		else if (tokenizer->GetCurToken().GetKeyword().Get() == KWDTP::KDO)
			CompileDo();
		else if (tokenizer->GetCurToken().GetKeyword().Get() == KWDTP::KRETURN)
			CompileReturn();
	}
	closeXMLTag("</statements>");
}

void CompileEngine::CompileSubroutineCall()
{
	std::string subRoutineName = className + "." + curSubRoutine.name;
	if (curSubRoutine.type == KWDTP::KDO &&
		curSubRoutine.name != sytb->TypeOf(curSubRoutine.name)) m_arg = 1;
	else m_arg = 0;

	switch (tokenizer->GetCurToken().GetType())
	{
	case TYPE::T_SYMBOLS:
		if (tokenizer->GetCurToken().GetTokenName() == ".") {
			Eat(".");
			tokenizer->GetNextToken();
			Eat("identifier");
			
			if (sytb->TypeOf(curSubRoutine.name) != "" && !Token(sytb->TypeOf(curSubRoutine.name)).GetKeyword().IsVarType()
				&& curSubRoutine.name != sytb->TypeOf(curSubRoutine.name)) {
				vm->writePush(Segment::THIS, sytb->IndexOf(curSubRoutine.name));
				m_arg = 1;
				subRoutineName = sytb->TypeOf(curSubRoutine.name) + "." + tokenizer->GetCurToken().GetRealName();
			}
			else {
				subRoutineName = curSubRoutine.name + "." + tokenizer->GetCurToken().GetRealName();
			}

			tokenizer->GetNextToken();
			Eat("(");
			tokenizer->GetNextToken();
			CompileExpressionList();
			vm->writeCall(subRoutineName , m_arg);
			Eat(")");
		}
		else if (tokenizer->GetCurToken().GetTokenName() == "(")
		{
			Eat("(");
			tokenizer->GetNextToken();
			m_arg = 1;
			//m_arg = 1;
			vm->writePush(Segment::POINTER, 0);
			CompileExpressionList();
			vm->writeCall(subRoutineName, m_arg);
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

	if (sytb->KindOf(tokenizer->GetCurToken().GetTokenName()) != Symbol::NONE) {
		curSubRoutine.type = KWDTP::KDO;
		vm->writePush((Segment)sytb->KindOf(tokenizer->GetCurToken().GetTokenName()), sytb->IndexOf(tokenizer->GetCurToken().GetTokenName()));
		curSubRoutine.name = sytb->TypeOf(tokenizer->GetCurToken().GetRealName());
	}
	else {
		curSubRoutine.name = tokenizer->GetCurToken().GetTokenName();
	}
	
	
	tokenizer->GetNextToken();
	CompileSubroutineCall();
	vm->writePop(Segment::TEMP, 0);
	Eat(";");
	tokenizer->GetNextToken();
	closeXMLTag("</doStatement>");
}

void CompileEngine::CompileLet()
{
	openXMLTag("<letStatement>");
	Eat("let");
	curSubRoutine.type = KWDTP::KLET;
	tokenizer->GetNextToken();
	switch (tokenizer->GetCurToken().GetType())
	{
	case TYPE::T_IDENTIFIER:
		Eat("identifier");
		std::string identifier = tokenizer->GetCurToken().GetTokenName();
		tokenizer->GetNextToken();
		if (tokenizer->GetCurToken().GetTokenName() == "=")
		{
			Eat("=");
			tokenizer->GetNextToken();
			CompileExpression();
			vm->writePop((Segment)sytb->KindOf(identifier), sytb->IndexOf(identifier));
			Eat(";");
			tokenizer->GetNextToken();
		}
		else if (tokenizer->GetCurToken().GetTokenName() == "[")
		{
			Eat("[");
			tokenizer->GetNextToken();
			CompileExpression();
			vm->writePush((Segment)sytb->KindOf(identifier), sytb->IndexOf(identifier));
			vm->WriteAritmetic(Operation::ADD);
			Eat("]");
			tokenizer->GetNextToken();
			if (tokenizer->GetCurToken().GetTokenName() == "=")
			{
				Eat("=");
				tokenizer->GetNextToken();
				std::string curToken = tokenizer->GetCurToken().GetTokenName();
				CompileExpression();
				if (tokenizer->GetCurToken().GetTokenName() == ";")
					Eat(";");
				tokenizer->GetNextToken();

				vm->writePop(Segment::TEMP, 0);
				vm->writePop(Segment::POINTER, 1);
				vm->writePush(Segment::TEMP, 0);
				vm->writePop(Segment::THAT, 0);
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
		tokenizer->GetCurToken().GetType() == TYPE::T_INT_CONST ||
		tokenizer->GetCurToken().GetType() == TYPE::T_STRING_CONST ||
		tokenizer->GetCurToken().GetType() == TYPE::T_IDENTIFIER ) {
		CompileExpression();
	}
	else
		vm->writePush(Segment::CONSTA, 0);
	vm->writeReturn();
	Eat(";");
	tokenizer->GetNextToken();
	closeXMLTag("</returnStatement>");
		
}

void CompileEngine::CompileIf()
{
	std::string labelTrue("IF_TRUE" + std::to_string(m_ifCount));
	std::string labelFalse("IF_FALSE" + std::to_string(m_ifCount));
	std::string labelEnd("IF_END" + std::to_string(m_ifCount));
	m_ifCount++;

	openXMLTag("<ifStatement>");
	Eat("if");
	tokenizer->GetNextToken();
	Eat("(");
	tokenizer->GetNextToken();
	CompileExpression();

	vm->WriteIf(labelTrue);
	vm->WriteGoto(labelFalse);
	vm->WriteLabel(labelTrue);
	
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
			vm->WriteLabel(labelEnd);
			CompileIf();
			break;
		default:
			vm->WriteGoto(labelEnd);
			vm->WriteLabel(labelFalse);
			Eat("{");
			tokenizer->GetNextToken();
			CompileStatements();
			Eat("}");
			vm->WriteLabel(labelEnd);
			tokenizer->GetNextToken();
			break;
		}
		break;
	default:
		vm->WriteLabel(labelFalse);
		break;
	}
	closeXMLTag("</ifStatement>");
}

void CompileEngine::CompileWhile()
{
	//'while' '(' expression ')' '{' statements '}'
	
	std::string labelTrue("WHILE_EXP" + std::to_string(m_whileCount));
	std::string labelEnd("WHILE_END" + std::to_string(m_whileCount));
	m_whileCount++;
	
	vm->WriteLabel(labelTrue);
	openXMLTag("<whileStatement>");
	Eat("while");
	
	tokenizer->GetNextToken();
	Eat("(");
	tokenizer->GetNextToken();
	CompileExpression();
	vm->WriteAritmetic(Operation::NOT);
	vm->WriteIf(labelEnd);
	Eat(")");
	tokenizer->GetNextToken();
	Eat("{");
	tokenizer->GetNextToken();
	CompileStatements();
	vm->WriteGoto(labelTrue);
	vm->WriteLabel(labelEnd);
	Eat("}");
	tokenizer->GetNextToken();
	closeXMLTag("</whileStatement>");
}


void CompileEngine::CompileExpression()
{
	//vm->writePush(Segment::ARG, 0);
	//vm->writePop(Segment::POINTER, 0);
	openXMLTag("<expression>");
	CompileTerm();
	while (tokenizer->GetCurToken().IsOp())
	{
		Eat(tokenizer->GetCurToken().GetTokenName());
		std::string op = tokenizer->GetCurToken().GetRealName();
		if (op == "~") vm->write("aqui");

		tokenizer->GetNextToken();
		CompileTerm();
		if(op == "+") vm->WriteAritmetic(Operation::ADD);
		else if(op == "-") vm->WriteAritmetic(Operation::SUB);
		else if(op == "|") vm->WriteAritmetic(Operation::OR);
		else if(op == "=") vm->WriteAritmetic(Operation::EQ);
		else if(op == "&") vm->WriteAritmetic(Operation::AND);
		else if(op == "~") vm->WriteAritmetic(Operation::NOT);
		else if(op == "<") vm->WriteAritmetic(Operation::LT);
		else if(op == ">") vm->WriteAritmetic(Operation::GT);
		else if(op == "*") vm->writeCall("Math.multiply",2);
		else if(op == "/") vm->writeCall("Math.divide",2);
	}
	closeXMLTag("</expression>");
}

void CompileEngine::CompileTerm()
{
	openXMLTag("<term>");
	std::string old = curSubRoutine.name;
	switch (tokenizer->GetCurToken().GetType())
	{
		case TYPE::T_IDENTIFIER:
			Eat("identifier");

			curSubRoutine.name = tokenizer->GetCurToken().GetTokenName();
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
				vm->writePush((Segment)sytb->KindOf(curSubRoutine.name), sytb->IndexOf(curSubRoutine.name));
				vm->WriteAritmetic(Operation::ADD);
				vm->writePop(Segment::POINTER, 1);
				vm->writePush(Segment::THAT, 0);
				Eat("]");
			}
			else
				vm->writePush((Segment)sytb->KindOf(curSubRoutine.name), sytb->IndexOf(curSubRoutine.name));

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
				std::string opU = tokenizer->GetCurToken().GetRealName();
				Eat(tokenizer->GetCurToken().GetTokenName());
				tokenizer->GetNextToken();
				CompileTerm();
				if(opU == "-")
					vm->WriteAritmetic(Operation::NEG);
				else if(opU == "~")
					vm->WriteAritmetic(Operation::NOT);
				closeXMLTag("</term>");
				return;
			}
		break;
		
		case TYPE::T_KEYWORDS:
			if (tokenizer->GetCurToken().GetKeyword().IsKeywordConstant())
			{
				Eat(tokenizer->GetCurToken().GetTokenName());
				KWDTP kwd = tokenizer->GetCurToken().GetKeyword().Get();
				if (kwd == KWDTP::KTRUE || kwd == KWDTP::KFALSE || kwd == KWDTP::KNULL) {
					vm->writePush(Segment::CONSTA, 0);
					if(kwd == KWDTP::KTRUE)
						vm->WriteAritmetic(Operation::NOT);
				}
				else
					vm->writePush(Segment::POINTER, 0);
			}
			break;
		case TYPE::T_INT_CONST:
			Eat(tokenizer->GetCurToken().GetTokenName());
			vm->writePush(Segment::CONSTA, std::stoi(tokenizer->GetTokenName().c_str()));
			break;
		case TYPE::T_STRING_CONST:
			Eat(tokenizer->GetCurToken().GetTokenName());
			vm->writePush(Segment::CONSTA, tokenizer->GetCurToken().GetTokenName().size());
			vm->writeCall("String.new", 1);
			for (unsigned i = 0; i < tokenizer->GetCurToken().GetTokenName().size(); i++)
			{
				vm->writePush(Segment::CONSTA, tokenizer->GetCurToken().GetTokenName()[i]);
				vm->writeCall("String.appendChar", 2);
			}
			break;
	default:
		break;
	}
	curSubRoutine.name = old;
	tokenizer->GetNextToken();
	closeXMLTag("</term>");
}

void CompileEngine::CompileExpressionList()
{

	openXMLTag("<expressionList>");
	if (tokenizer->GetCurToken().GetType() != TYPE::T_SYMBOLS || tokenizer->GetCurToken().GetTokenName() == "(") {
		CompileExpression();
		m_arg++;
	}
	while (tokenizer->GetCurToken().GetTokenName() == ",")
	{
		Eat(",");
		tokenizer->GetNextToken();
		m_arg++;
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
	//printf("%s%s\n", std::string(m_spaces * 2, ' ').c_str() , str.c_str());
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