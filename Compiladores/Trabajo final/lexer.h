#ifndef LEXER_H
#define LEXER_H

#include "tokens.h"
#include <string>
#include <fstream>
#include <unordered_map>

class Lexer {
    std::string src;
    size_t pos = 0;
    int line = 1;

public:
    Lexer(std::ifstream &in);

    Token next();
    Token peek();

private:
    bool eof() const;
    char current() const;
    char advance();
    bool match(char expected);

    void skipSpaces();
    Token identifier();
    Token number();
    Token stringLiteral();
};

#endif
