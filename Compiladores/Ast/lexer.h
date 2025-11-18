#ifndef COMPILER_LEXER
#define COMPILER_LEXER

#include <unordered_map>
#include <string>
#include <sstream>
using std::stringstream;
using std::unordered_map;
using std::string;

// cada token tiene una Tag (un número a partir de 256)
// la Tag de los caracteres individuales es su código ASCII
enum Tag { ID = 256, INTEGER, REAL, TYPE, TRUE, FALSE, MAIN, IF, WHILE, DO, OR, AND, EQ, NEQ, LTE, GTE };

// clase para representar tokens
struct Token
{
	int tag;
	string lexeme;

	Token() : tag(0) {}
	Token(char ch) : tag(int(ch)), lexeme({ch}) {}
	Token(int t, string s) : tag(t), lexeme(s) {}
};

// analizador léxico
class Lexer
{
private:
	char peek;			// último caracter leido
	Token token;		// último token retornado
	int line = 1;		// número de la linea actual

	// tabla para identificadores y palabras-clave
	unordered_map<string, Token> token_table;

public:
	Lexer();			// constructor
	int Lineno();		// retorna linea actual
	Token * Scan();		// retorna próximo token de la entrada
};

#endif
