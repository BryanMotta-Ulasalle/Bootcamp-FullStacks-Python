#ifndef TOKENS_H
#define TOKENS_H

#include <string>

enum class TokenType {
    // Palabras clave y literales
    FUN, END, ELSE, IF, WHILE, LOOP, RETURN,
    TRUE_, FALSE_,
    NEW_, AND_, OR_, NOT_,

    // Tipos
    INT, BOOL, CHAR, STRING,

    // Literales y IDs
    LITNUMERAL, LITSTRING,
    ID,

    // Símbolos
    LPAREN, RPAREN,
    LBRACKET, RBRACKET,
    COLON, COMMA, ASSIGN,

    PLUS, MINUS, STAR, SLASH,

    GT, LT, GE, LE, EQ, NE,

    // Saltos de línea (NL) y fin de archivo
    NL, END_OF_FILE,

    // Error / inválido
    INVALID
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;

    Token(TokenType t=TokenType::INVALID, const std::string &l="", int ln=0)
        : type(t), lexeme(l), line(ln) {}
};

#endif // TOKENS_H
