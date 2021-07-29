// main.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Tokenizer.h"
#include "LexerXML.h"
#include "CompileEngine.h"

int main(int argc, char* argv[])
{
	unsigned char i = 0;

	if (argc < 2) {
		printf("inicializacao sem linha de comando\n");
	}
	else {
		//std::string cmd("JoinLess.jack");
		Lexer lexer(argv[1]);
		CompileEngine sintaxer(argv[1]);
	}
	printf("pressione quaquer tecla para sair\n");
	i = getchar();	
	return 0;
}