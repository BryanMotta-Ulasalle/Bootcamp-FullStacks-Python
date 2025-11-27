#ifndef LEXER_H
#define LEXER_H

#include "tokens.h"
#include <string>
#include <fstream>

class Lexer {
    std::string src;
    size_t pos;
    int line;

public:
    Lexer(std::ifstream &in);

    // Devuelve siguiente token y avanza
    Token next();

    // Mira siguiente token sin consumir
    Token peek();

private:
    bool eof() const;
    char current() const;
    char advance();
    bool match(char expected);

    void skipSpacesExceptNewline();
    Token identifierOrKeyword(const std::string &word);
    Token identifier();
    Token number();
    Token stringLiteral();
};

#endif // LEXER_H
