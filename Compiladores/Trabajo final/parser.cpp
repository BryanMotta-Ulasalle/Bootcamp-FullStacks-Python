#include "parser.h"

Parser::Parser(Lexer &l) : lex(l) {
    current = lex.next();
}

void Parser::advance() {
    current = lex.next();
}

void Parser::expect(TokenType t, const std::string &msg) {
    if (current.type != t) {
        throw std::runtime_error(
            "Error en línea " + std::to_string(current.line) +
            ": " + msg + " Se esperaba token distinto."
        );
    }
    advance();
}

void Parser::parse() {
    programa();
}

void Parser::programa() {
    while (current.type == TokenType::NL) advance();
    while (current.type != TokenType::END_OF_FILE) {
        decl();
    }
}

void Parser::decl() {
    if (current.type == TokenType::FUN)
        funcion();
    else
        declvar();
}

void Parser::funcion() {
    expect(TokenType::FUN, "Se esperaba 'fun'");
    expect(TokenType::ID, "Nombre de función esperado");
    expect(TokenType::LPAREN, "Falta '('");
    while (current.type != TokenType::RPAREN) advance();
    expect(TokenType::RPAREN, "Falta ')'");
    expect(TokenType::NL, "Se esperaba salto de línea");
    bloque();
    expect(TokenType::END, "Se esperaba 'end'");
    expect(TokenType::NL, "Se esperaba salto de línea");
}

void Parser::bloque() {
    while (current.type == TokenType::ID) {
        declvar();
        expect(TokenType::NL, "Se esperaba salto de línea");
    }
    while (current.type != TokenType::END &&
           current.type != TokenType::ELSE &&
           current.type != TokenType::LOOP) {
        comando();
        expect(TokenType::NL, "Se esperaba salto de línea");
    }
}

void Parser::declvar() {
    expect(TokenType::ID);
    expect(TokenType::COLON);
    advance(); // simplificado
}

void Parser::comando() {
    if (current.type == TokenType::RETURN)
        advance();
    else
        exp();
}

//////////////////////
// EXPRESIONES
//////////////////////

void Parser::exp()       { exp_or(); }
void Parser::exp_or()    { exp_and(); }
void Parser::exp_and()   { exp_cmp(); }
void Parser::exp_cmp()   { exp_add(); }
void Parser::exp_add()   { exp_mul(); }
void Parser::exp_mul()   { exp_unary(); }

void Parser::exp_unary() {
    if (current.type == TokenType::MINUS ||
        current.type == TokenType::NOT_) {
        advance();
    }
    exp_primary();
}

void Parser::exp_primary() {
    if (current.type == TokenType::LITNUMERAL ||
        current.type == TokenType::LITSTRING ||
        current.type == TokenType::TRUE_ ||
        current.type == TokenType::FALSE_) {
        advance();
        return;
    }

    if (current.type == TokenType::ID) {
        advance();
        return;
    }

    if (current.type == TokenType::LPAREN) {
        advance();
        exp();
        expect(TokenType::RPAREN, "Falta ')'");
        return;
    }

    throw std::runtime_error(
        "Expresión inválida en línea " +
        std::to_string(current.line)
    );
}
