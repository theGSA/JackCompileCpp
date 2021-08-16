#pragma once
#include "stdafx.h"
#include <string>

enum class Segment
{
	CONSTA,
	ARG,
	LOCAL,
	STATIC,
	THIS,
	POINTER,
	TEMP,
	THAT
};

enum class Operation
{
	ADD,
	SUB,
	NEG,
	EQ,
	GT,
	LT,
	AND,
	OR,
	NOT
};
class VMWriter
{
public:
	VMWriter(std::string filename);
	void writePush(Segment seg, int Index);
	void writePop(Segment seg, int index);
	void WriteAritmetic(Operation op);
	void WriteLabel(std::string);
	void WriteGoto(std::string);
	void WriteIf(std::string);
	void writeCall(std::string name, int nArgs);
	void writeFunction(std::string, int nLocals);
	void writeReturn();
	void write(std::string str);
	void close();
private:
	FILE* m_arq;
	std::string m_filename;
};

