#include <iostream>
#include <fstream>
#include <cstring>
#include "lexer.h"
#include "parser.h"
#include "error.h"
#include "ast.h"
#include "checker.h"
using namespace std;

ifstream fin;
Lexer * scanner;

// programa puede recibir nombres de archivos
int main(int argc, char **argv)
{
	if (argc == 2)
	{
		fin.open(argv[1]);
		if (!fin.is_open())
		{
			cout << "Falla en la abertura del archivo \'" << argv[1] << "\'.\n";
			exit(EXIT_FAILURE);
		}

		//TestLexer();
		Lexer lector;
		scanner = &lector;
		Node * ast;		
		Parser traductor;
		try
		{
			ast = traductor.Start();
		}
		catch (SyntaxError err)
		{
			err.What();
		}
		fin.close();
		TestParser(ast);
	}
}
