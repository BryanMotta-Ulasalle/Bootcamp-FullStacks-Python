#ifndef TOKENS_H
#define TOKENS_H

#include <string>

enum class TokenType {
    // Palabras clave
    FUN, END, ELSE, IF, WHILE, LOOP, RETURN,
    TRUE_, FALSE_,
    NEW_, AND_, OR_, NOT_,

    // Tipos
    INT, BOOL, CHAR, STRING,

    // Literales
    LITNUMERAL, LITSTRING,

    // Identificadores
    ID,

    // Símbolos
    LPAREN, RPAREN,
    LBRACKET, RBRACKET,
    COLON, COMMA, ASSIGN,

    PLUS, MINUS, STAR, SLASH,

    GT, LT, GE, LE, EQ, NE,

    NL,
    END_OF_FILE,
    INVALID
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;

    Token(TokenType t=TokenType::INVALID, std::string l="", int li=0)
        : type(t), lexeme(l), line(li) {}
};

#endif
