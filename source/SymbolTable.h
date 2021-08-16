#pragma once
#include <string>
#include <vector>
#include "VMWriter.h"

enum class Symbol
{
    UNDEF,
    ARG,
    VAR,
    STATIC,
    FIELD,
    NONE
};


typedef struct
{
    std::string name;
    std::string type;
    Symbol kind;
}SymbolTableDesc;

class SymbolTable
{
public:
    SymbolTable();
    void startSubroutine();
    void define(std::string name, std::string type, Symbol kind);
    int VarCount(Symbol kind);
    Symbol KindOf(std::string name);
    std::string TypeOf(std::string name);
    int IndexOf(std::string name);
private:
    std::vector <SymbolTableDesc>classScope;
    std::vector <SymbolTableDesc>subroutineScope;
    int scope;
};

