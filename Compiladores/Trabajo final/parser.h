#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include <stdexcept>

class Parser {
    Lexer &lex;
    Token current;

public:
    Parser(Lexer &l);
    void parse();

private:
    void advance();
    void expect(TokenType t, const std::string &msg="");

    // Reglas principales
    void programa();
    void decl();
    void funcion();
    void bloque();
    void declvar();
    void comando();

    // Expresiones
    void exp();
    void exp_or();
    void exp_and();
    void exp_cmp();
    void exp_add();
    void exp_mul();
    void exp_unary();
    void exp_primary();
};

#endif
