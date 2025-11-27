#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include <stdexcept>
#include <string>

class Parser {
    Lexer &lex;
    Token cur;

public:
    Parser(Lexer &l);
    // Punto de entrada público
    void programa();

private:
    void advance();
    Token peek();
    void expect(TokenType t, const std::string &msg="");

    // Reglas
    void decl();
    void funcion();
    void global_decl(); // declvar NL
    void bloque();
    void declvar();
    void comando();
    void cmdif();
    void cmdwhile();
    void cmdatrib();
    void cmdreturn();
    void llamada(); // ID ( listaexp )
    void listaexp();

    // Expresiones (jerarquía)
    void exp();         // or_exp
    void or_exp();
    void and_exp();
    void cmp_exp();
    void sum_exp();
    void mul_exp();
    void unary_exp();
    void primary();
    void var_access();
};

#endif // PARSER_H
