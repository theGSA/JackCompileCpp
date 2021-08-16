// main.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Tokenizer.h"
#include "LexerXML.h"
#include "CompileEngine.h"
#include "JackCompiler.h"

int main(int argc, char* argv[])
{
	unsigned char i = 0;
	argc = 2;
	if (argc < 2) {
		printf("inicializacao sem linha de comando\n");
	}
	else {
		JackCompiler app(argc, argv);
	}
	printf("pressione quaquer tecla para sair\n");
	i = getchar();	
	return 0;
}