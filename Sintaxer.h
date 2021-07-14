#pragma once
#include <string>
#include "Tokenizer.h"

class  Sintaxer
{
public:
	 Sintaxer();
	 Sintaxer(std::string filename);
	~ Sintaxer();

private:
	FILE* m_output;
	Tokenizer* tokenizer;
	//void Init();
	void CompileClass();
	void CompileClassVarDec();
	void CompileSubroutine();
	void CompileSubroutineBody();
	void CompileParameterList();
	void CompileVarDec();
	void CompileStatements();
	void CompileDo();
	void CompileLet();
	void CompileReturn();
	void CompileIf();
	void CompileWhile();
	void CompileExpression();
	void CompileTerm();
	void CompileExpressionList();
	void WriteFile(std::string str);
};
