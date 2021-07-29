#pragma once
#include <string>
#include "Tokenizer.h"

class  CompileEngine
{
public:
	 CompileEngine();
	 CompileEngine(std::string filename);
	~ CompileEngine();

private:
	FILE* m_output;
	Tokenizer* tokenizer;
	std::string m_filename;
	int m_spaces;
	
	void Compile();
	void CompileClass();
	void CompileClassVarDec();
	void CompileSubroutine();
	void CompileSubroutineParameters();
	void CompileSubroutineBody();
	void CompileParameterList();
	void CompileVarDec();
	void CompileStatements();
	void CompileDo();
	void CompileLet();
	void CompileLetAssignments();
	void CompileReturn();
	void CompileIf();
	void CompileWhile();
	void CompileExpression();
	void CompileSubroutineCall();
	void CompileTerm();
	void CompileExpressionList();
	void WriteFile(std::string str);
	//
	void Eat(std::string str);
	void openXMLTag(std::string str);
	void closeXMLTag(std::string str);
};
