// AnalizadorLexico.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Tokenizer.h"
#include "LexerXML.h"
#include "Sintaxer.h"

int main(int argc, char* argv[])
{
	unsigned i = 0;
	//Tokenizer Token();

	/*if (argc < 2) {
		printf("inicializacao sem linha de comando\n");
		return 0;
	}*/
	char* file = "Main.jack";
	//Lexer lexer(argv[1]);
	Lexer lexer(file);
	//Tokenizer Token2("Square.jack");
	Sintaxer  sintaxer(file);
	printf("pressione quaquer tecla para sair\n");
	i = getchar();	
	return 0;
}

