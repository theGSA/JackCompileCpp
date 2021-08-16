#include "SymbolTable.h"

SymbolTable::SymbolTable():
	scope(-1)
{
	classScope.clear();
	subroutineScope.clear();
}

std::string KindToString(Symbol kind)
{
	const char* strKinds[6] = { "undefined", "argument", "local", "static", "this", "none"};

	std::string aux = strKinds[(int)kind];

	return aux;
}
void SymbolTable::startSubroutine()
{
	subroutineScope.clear();
}
void SymbolTable::define(std::string name, std::string type, Symbol kind)
{
	SymbolTableDesc temp = { name = name, type = type, kind = kind};
	//printf("define: name(%s), type(%s), kind:(%s) ", name.c_str(), type.c_str(), KindToString(kind).c_str());
	if (kind == Symbol::STATIC || kind == Symbol::FIELD)
	{
		classScope.push_back(temp);
		//printf("index(%d)(CS)\n", IndexOf(name));
	}
	else if (kind == Symbol::ARG || kind == Symbol::VAR)
	{
		subroutineScope.push_back(temp);
		//printf("index(%d)(SS)\n", IndexOf(name));
	}
}

int SymbolTable::VarCount(Symbol kind)
{
	int count = 0;
	
	if (kind == Symbol::STATIC || kind == Symbol::FIELD)
	{
		for (size_t i = 0; i < classScope.size(); i++)
			if(classScope[i].kind == kind)
				count++;
	}
	else if (kind == Symbol::ARG || kind == Symbol::VAR)
	{
		for (size_t i = 0; i < subroutineScope.size(); i++)
			if (subroutineScope[i].kind == kind)
				count++;
	}
	return count;
}
Symbol SymbolTable::KindOf(std::string name)
{

	for (size_t i = 0; i < subroutineScope.size(); i++)
		if (subroutineScope[i].name == name)
			return subroutineScope[i].kind;

	for (size_t i = 0; i < classScope.size(); i++)
		if (classScope[i].name == name)
			return classScope[i].kind;

	return Symbol::NONE;
}
std::string SymbolTable::TypeOf(std::string name)
{
	std::vector<std::string> soType = { "Memory", "Sys", "Screen", "Output" };

	if (std::find(soType.begin(), soType.end(), name) != soType.end())
		return name;

	for (size_t i = 0; i < subroutineScope.size(); i++)
		if (subroutineScope[i].name == name)
			return subroutineScope[i].type;
	
	for (size_t i = 0; i < classScope.size(); i++)
		if (classScope[i].name == name)
			return classScope[i].type;
	return "";
}
int SymbolTable::IndexOf(std::string name)
{
	Symbol kind = KindOf(name);
	int index = 0;

	if (kind == Symbol::NONE)
	{
		printf("tipo nâo encontrado(%s)\n", name.c_str());
		//exit(0);
	}
	
	for (size_t i = 0; i < subroutineScope.size(); i++) {
		if (subroutineScope[i].kind == kind) {
			if (subroutineScope[i].name == name)
				return index;
			index++;
		}
	}
	for (size_t i = 0; i < classScope.size(); i++) {
		if (classScope[i].kind == kind) {
			if (classScope[i].name == name)
				return index;
			index++;
		}
	}
	return -1;
}