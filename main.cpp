// AnalizadorLexico.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Tokenizer.h"

int main(int argc, char* argv[])
{
	unsigned i = 0;
	//Tokenizer Token();

	if (argc < 2) {
		printf("inicializacao sem linha de comando\n");
		return 0;
	}

	Tokenizer Token(argv[1]);
	//Tokenizer Token2("Square.jack");
	printf("pressione quaquer tecla para sair\n");
	i = getchar();
	return 0;
}

