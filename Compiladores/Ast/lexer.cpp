#include "lexer.h"
#include <fstream>
#include <sstream>
using std::stringstream;

extern std::ifstream fin;

// constructor 
Lexer::Lexer()
{
	// inserta palabras-reservadas en la tabla
	token_table["main"]  = Token{ Tag::MAIN, "main" };
	token_table["int"]   = Token{ Tag::TYPE, "int" };
	token_table["float"] = Token{ Tag::TYPE, "float" };
	token_table["bool"] = Token{ Tag::TYPE, "bool" };
	token_table["true"]  = Token{ Tag::TRUE, "true" };
	token_table["false"] = Token{ Tag::FALSE, "false" };
	token_table["if"]    = Token{ Tag::IF,    "if" };
	token_table["while"] = Token{ Tag::WHILE, "while" };
	token_table["do"]    = Token{ Tag::DO,    "do" };
	
	// inicia lectura de la entrada
	peek = fin.get();
}

// retorna número de linea actual
int Lexer::Lineno()
{
	return line;
}

// retorna tokens para el analizador sintáctico
Token * Lexer::Scan()
{
	// ignora espacios en blanco, tabulaciones y nuevas lineas
	while (isspace(peek))
	{
		if (peek == '\n')
			line += 1;
		peek = fin.get();
	}

	// ignora comentarios
	while (peek == '/')
	{
		peek = fin.get();
		if (peek == '/')
		{
			// ignora caracteres hasta el fin de la linea
			do
				peek = fin.get();
			while(peek != '\n');
			line += 1;
			peek = fin.get();
		}
		else if (peek == '*')
		{
			// ignora caracteres hasta hallar */ o EOF				
			while( (peek=fin.get()) != '*' ||  (peek=fin.get()) != '/' )
			{
				if (peek == '\n')
				{
					line += 1;
				}
				else if (peek == EOF)
				{
					token = Token {EOF};
					return &token;
				}
			}
			peek = fin.get();	
		}
		else
		{
			// barra encontrada no inicia un comentario
			fin.unget();
			peek = '/';
			break;
		}

		// remueve espacios en blanco, tabulaciones y nuevas linhas
		while (isspace(peek))
		{
			if (peek == '\n')
				line += 1;
			peek = fin.get();
		}
	}

	// retorna números
	if (isdigit(peek))
	{
		// punto-flotante no fue encontrado
		bool dot = false;
		
		// acumula dígitos del número
		stringstream ss;
		do 
		{
			if (peek == '.')
			{
				if (dot == false)
				{
					// primer punto encontrado
					dot = true;
				}
				else
				{
					// segundo punto encontrado
					break; 
				}
			}

			ss << peek;
			peek = fin.get();
		} 
		while (isdigit(peek) || peek == '.');

		// si el número es un punto-flotante
		if (dot)
		{
			token = Token{Tag::REAL, ss.str()};
			return &token;
		}
		else
		{
			token = Token{Tag::INTEGER, ss.str()};
			return &token;
		}
	}

	// retorna palabras-clave e identificadores
	if (isalpha(peek))
	{
		stringstream ss;
		do 
		{
			ss << peek;
			peek = fin.get();
		} 
		while (isalpha(peek));

		string s = ss.str();
		auto pos = token_table.find(s);

		// si el lexema ya está en la tabla
		if (pos != token_table.end())
		{
			// retorna el token de la tabla
			token = pos->second;
			return &token;
		}

		// si el lexema todavia no está en la tabla
		Token t {Tag::ID, s};
		token_table[s] = t;

		// retorna el token ID
		token = t;
		return &token;
	}

	// retorna operadores con mas de un caracter: >=, <=, == e !=
	switch(peek)
	{
		case '&':
		{
			char next = fin.get();
			if (next == '&')
			{
				peek = fin.get();
				token = Token{Tag::AND, "&&"};
				return &token;
			}
			else
			{
				fin.unget();
			}
		}
		break;

		case '|':
		{
			char next = fin.get();
			if (next == '|')
			{
				peek = fin.get();
				token = Token{Tag::OR, "||"};
				return &token;
			}
			else
			{
				fin.unget();
			}
		}
		break;

		case '>':
		{
			char next = fin.get();
			if (next == '=')
			{
				peek = fin.get();
				token = Token{Tag::GTE, ">="};
				return &token;
			}
			else
			{
				fin.unget();
			}
		}
		break;

		case '<':
		{
			char next = fin.get();
			if (next == '=')
			{
				peek = fin.get();
				token = Token{Tag::LTE, "<="};
				return &token;
			}
			else
			{
				fin.unget();
			}
		}
		break;

		case '=':
		{
			char next = fin.get();
			if (next == '=')
			{
				peek = fin.get();
				token = Token{Tag::EQ, "=="};
				return &token;
			}
			else
			{
				fin.unget();
			}
		}
		break;

		case '!':
		{
			char next = fin.get();
			if (next == '=')
			{
				peek = fin.get();
				token = Token{Tag::NEQ, "!="};
				return &token;	
			}
			else
			{
				fin.unget();
			}
		}
		break;
	}

	// retorna caracteres no alphanuméricos aislados: (, ), +, -, etc.
	token = Token{peek};
	peek = ' ';
	return &token;
}
