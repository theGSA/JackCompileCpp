#include <iostream>
#include "VMWriter.h"
#include "utils.h"
#include <unistd.h>
#include <sys/stat.h >
#include <sys/types.h>
#include <dirent.h>

const char* strSegment[] =
{
	"constant",
	"argument",
	"local",
	"static",
	"this",
	"pointer",
	"temp",
	"that"
};

VMWriter::VMWriter(std::string filename)
{	
	m_filename = filename;

	m_arq = fopen(m_filename.c_str(), "w");
	if (!m_arq)
	{
		printf("não pode criar o arquivo %s\n", m_filename.c_str());
		exit(0);
	}
	else {
		printf("criou o arquivo %s\n", m_filename.c_str());
	}
	//fclose(m_arq);
}
void VMWriter::writePush(Segment seg, int index)
{
	write("push " + std::string(strSegment[(int)seg]) + " " + std::to_string(index));
}

void VMWriter::writePop(Segment seg, int index)
{
	write("pop " + std::string(strSegment[(int)seg]) + " " + std::to_string(index));
}

void VMWriter::WriteAritmetic(Operation op)
{
	const char* opr[] = { "add", "sub", "neg", "eq" , "gt", "lt", "and", "or", "not" };
	write(opr[(int)op]);
}

void VMWriter::WriteLabel(std::string str)
{
	write("label " + str);
}
void VMWriter::WriteGoto(std::string str) 
{
	write("goto " + str);
}
void VMWriter::WriteIf(std::string str)
{
	write("if-goto " + str);

}
void VMWriter::writeCall(std::string name, int nArgs)
{
	write("call "+name+" "+std::to_string(nArgs));
}
void VMWriter::writeFunction(std::string func, int nLocals)
{
	write("function "+func + " " +std::to_string(nLocals));
}
void VMWriter::writeReturn()
{
	write("return");
}
void VMWriter::write(std::string str)
{
	m_arq = fopen(m_filename.c_str(), "a+");
	fprintf(m_arq, "%s\n", str.c_str());
	std::cout << str << std::endl;
	fclose(m_arq);
}
void VMWriter::close()
{
	fclose(m_arq);
}