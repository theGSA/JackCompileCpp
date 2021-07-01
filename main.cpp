// AnalizadorLexico.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Tokenizer.h"

int main(int argc, char* argv[])
{
	unsigned i = 0;

	if (argc > 1)
		Tokenizer Token(argv[1]);
	else
		printf("inicializacao sem linha de comando\n");

	Tokenizer Token("Main.jack");
	Tokenizer Token2("Square.jack");
	printf("pressione quaquer tecla para sair\n");
	i = getchar();
	return 0;
}

