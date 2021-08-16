#pragma once
#include <string>
#include "Tokenizer.h"
#include "VMWriter.h"
#include "SymbolTable.h"
typedef struct 
{
	std::string name;
	KWDTP type;
}funcDesc;


class  CompileEngine
{
public:
	 CompileEngine();
	 CompileEngine(std::string filename);
	~ CompileEngine();

private:
	FILE* m_output;
	Tokenizer* tokenizer;
	VMWriter* vm;
	SymbolTable* sytb;
	std::string m_filename;
	std::string className;
	funcDesc curSubRoutine;
	int m_spaces;
	int m_arg;
	int m_ifCount;
	int m_whileCount;

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
	//
	
};
