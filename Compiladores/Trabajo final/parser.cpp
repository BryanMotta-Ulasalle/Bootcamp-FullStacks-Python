//parser.cpp
#include "parser.h"
#include <iostream>

Parser::Parser(Lexer &l) : lex(l) {
    cur = lex.next();
}

void Parser::advance() { cur = lex.next(); }
Token Parser::peek() { return lex.peek(); }

void Parser::expect(TokenType t, const std::string &msg) {
    if (cur.type != t) {
        std::string m = "Error en linea " + std::to_string(cur.line) + ": " + msg;
        throw std::runtime_error(m);
    }
    advance();
}

// --------------------
// programa = { NL } { decl } EOF
// --------------------
void Parser::programa() {
    
    while (cur.type == TokenType::NL) advance();


    while (cur.type != TokenType::END_OF_FILE) {
        if (cur.type == TokenType::NL) {
            advance(); 
            continue;
        }

        if (cur.type == TokenType::FUN || cur.type == TokenType::ID) {
            decl();
            continue;
        }

    
        throw std::runtime_error("Error en linea " + std::to_string(cur.line) + ": Se esperaba declaraciin (fun o ID)");
    }
}

// decl = funcion | global
void Parser::decl() {
    if (cur.type == TokenType::FUN) {
        funcion();
    } else if (cur.type == TokenType::ID) {
        global_decl();
    } else {
        throw std::runtime_error("Error en linea " + std::to_string(cur.line) + ": Se esperaba declaracion (fun o ID)");
    }
}

// funcion = "fun" ID "(" params ")" [ ":" tipo ] NL bloque "end" NL
void Parser::funcion() {
    expect(TokenType::FUN, "Se esperaba 'fun'");
    expect(TokenType::ID, "Se esperaba identificador de funcion");
    expect(TokenType::LPAREN, "Se esperaba '('");
    
    if (cur.type != TokenType::RPAREN) {
        // parse parametros: ID ":" tipo { ',' ID ':' tipo }
        while (true) {
            if (cur.type != TokenType::ID) throw std::runtime_error("Error en linea " + std::to_string(cur.line) + ": Se esperaba nombre de parametro");
            advance();
            expect(TokenType::COLON, "Se esperaba ':' en parametro");
            if (!(cur.type == TokenType::INT || cur.type == TokenType::BOOL || cur.type == TokenType::CHAR || cur.type == TokenType::STRING)) {
                throw std::runtime_error("Error en linea " + std::to_string(cur.line) + ": Se esperaba tipo de parametro");
            }
            advance();
            if (cur.type == TokenType::COMMA) { advance(); continue; }
            break;
        }
    }
    expect(TokenType::RPAREN, "Se esperaba ')'");
    // optional : tipo
    if (cur.type == TokenType::COLON) {
        advance();
        if (!(cur.type == TokenType::INT || cur.type == TokenType::BOOL || cur.type == TokenType::CHAR || cur.type == TokenType::STRING))
            throw std::runtime_error("Error en linea " + std::to_string(cur.line) + ": Se esperaba tipo después de ':'");
        advance();
    }
    
    expect(TokenType::NL, "Se esperaba salto de linea despues del encabezado de funcion");
    // bloque
    bloque();
    expect(TokenType::END, "Se esperaba 'end' al finalizar la funcion");
    
    expect(TokenType::NL, "Se esperaba salto de linea despues de 'end'");
}

// global = declvar NL
void Parser::global_decl() {
    declvar();
    expect(TokenType::NL, "Se esperaba salto de linea despues de declaracion global");
}

// declvar = ID ":" tipo
void Parser::declvar() {
    expect(TokenType::ID, "Se esperaba identificador en declaracion de variable");
    expect(TokenType::COLON, "Se esperaba ':' en declaracion de variable");
    if (!(cur.type == TokenType::INT || cur.type == TokenType::BOOL || cur.type == TokenType::CHAR || cur.type == TokenType::STRING)) {
        throw std::runtime_error("Error en línea " + std::to_string(cur.line) + ": Se esperaba tipo en declaración de variable");
    }
    advance();
    
    while (cur.type == TokenType::LBRACKET) {
        advance();
        expect(TokenType::RBRACKET, "Se esperaba ']'");
    }
}

// bloque = { declvar NL } { comando NL }
void Parser::bloque() {
    
    while (cur.type == TokenType::ID) {
        Token nextTok = peek();
        if (nextTok.type == TokenType::COLON) {
            declvar();
            expect(TokenType::NL, "Se esperaba salto de linea después de declaracion de variable");
            continue;
        }
        break;
    }

    
    while (cur.type != TokenType::END && cur.type != TokenType::ELSE && cur.type != TokenType::LOOP && cur.type != TokenType::END_OF_FILE) {
        comando();
        expect(TokenType::NL, "Se esperaba salto de linea despues de comando");
    }
}

// comando = cmdif | cmdwhile | cmdatrib | cmdreturn | llamada ;
void Parser::comando() {
    if (cur.type == TokenType::IF) { cmdif(); return; }
    if (cur.type == TokenType::WHILE) { cmdwhile(); return; }
    if (cur.type == TokenType::RETURN) { cmdreturn(); return; }
    if (cur.type == TokenType::ID) {
        Token look = peek();
        if (look.type == TokenType::LPAREN) { llamada(); return; }
        cmdatrib();
        return;
    }
    throw std::runtime_error("Error en linea " + std::to_string(cur.line) + ": comando invalido");
}

// cmdif = "if" exp NL bloque { "else" "if" exp NL bloque } [ "else" NL bloque ] "end"
void Parser::cmdif() {
    expect(TokenType::IF, "Se esperaba 'if'");
    exp();
    expect(TokenType::NL, "Se esperaba salto de linea después de condicion 'if'");
    bloque();
    
    while (cur.type == TokenType::ELSE) {
        Token nxt = peek();
        if (nxt.type == TokenType::IF) {
            advance(); 
            expect(TokenType::IF, "Se esperaba 'if' después de 'else'");
            exp();
            expect(TokenType::NL, "Se esperaba salto de linea después de 'else if' condicion");
            bloque();
        } else break;
    }
    
    if (cur.type == TokenType::ELSE) {
        advance();
        expect(TokenType::NL, "Se esperaba salto de linea después de 'else'");
        bloque();
    }
    expect(TokenType::END, "Se esperaba 'end' al finalizar 'if'");
}

// cmdwhile = "while" exp NL bloque "loop"
void Parser::cmdwhile() {
    expect(TokenType::WHILE, "Se esperaba 'while'");
    exp();
    expect(TokenType::NL, "Se esperaba salto de linea despues de condicion 'while'");
    bloque();
    expect(TokenType::LOOP, "Se esperaba 'loop' al finalizar 'while'");
}

// cmdatrib = var "=" exp
void Parser::cmdatrib() {
    var_access();
    expect(TokenType::ASSIGN, "Se esperaba '=' en asignacion");
    exp();
}

// cmdreturn = "return" [ exp ]
void Parser::cmdreturn() {
    expect(TokenType::RETURN, "Se esperaba 'return'");
    if (!(cur.type == TokenType::NL || cur.type == TokenType::END_OF_FILE)) {
        exp();
    }
}

// llamada = ID "(" listaexp ")"
void Parser::llamada() {
    expect(TokenType::ID, "Se esperaba identificador en llamada");
    expect(TokenType::LPAREN, "Se esperaba '(' en llamada");
    listaexp();
    expect(TokenType::RPAREN, "Se esperaba ')' en llamada");
}

// listaexp = /* vacío */ | exp { "," exp }
void Parser::listaexp() {
    if (cur.type == TokenType::RPAREN) return;
    exp();
    while (cur.type == TokenType::COMMA) { advance(); exp(); }
}

// var = ID { "[" exp "]" }
void Parser::var_access() {
    expect(TokenType::ID, "Se esperaba identificador");
    while (cur.type == TokenType::LBRACKET) {
        advance();
        exp();
        expect(TokenType::RBRACKET, "Se esperaba ']'");
    }
}

// --------------------
// EXPRESIONES
// --------------------
void Parser::exp() { or_exp(); }

void Parser::or_exp() {
    and_exp();
    while (cur.type == TokenType::OR_) {
        advance();
        and_exp();
    }
}

void Parser::and_exp() {
    cmp_exp();
    while (cur.type == TokenType::AND_) {
        advance();
        cmp_exp();
    }
}

void Parser::cmp_exp() {
    sum_exp();
    while (cur.type == TokenType::EQ || cur.type == TokenType::NE || cur.type == TokenType::GT ||
           cur.type == TokenType::LT || cur.type == TokenType::GE || cur.type == TokenType::LE) {
        advance();
        sum_exp();
    }
}

void Parser::sum_exp() {
    mul_exp();
    while (cur.type == TokenType::PLUS || cur.type == TokenType::MINUS) {
        advance();
        mul_exp();
    }
}

void Parser::mul_exp() {
    unary_exp();
    while (cur.type == TokenType::STAR || cur.type == TokenType::SLASH) {
        advance();
        unary_exp();
    }
}

void Parser::unary_exp() {
    if (cur.type == TokenType::MINUS || cur.type == TokenType::NOT_) {
        advance();
        unary_exp();
        return;
    }
    primary();
}

void Parser::primary() {
    if (cur.type == TokenType::LITNUMERAL || cur.type == TokenType::LITSTRING ||
        cur.type == TokenType::TRUE_ || cur.type == TokenType::FALSE_) {
        advance();
        return;
    }
    if (cur.type == TokenType::ID) {
        Token nextTok = peek();
        if (nextTok.type == TokenType::LPAREN) {
            llamada();
            return;
        } else {
            var_access();
            return;
        }
    }
    if (cur.type == TokenType::NEW_) {
        advance();
        expect(TokenType::LBRACKET, "Se esperaba '[' después de 'new'");
        exp();
        expect(TokenType::RBRACKET, "Se esperaba ']' en 'new'");
        if (!(cur.type == TokenType::INT || cur.type == TokenType::BOOL || cur.type == TokenType::CHAR || cur.type == TokenType::STRING)) {
            throw std::runtime_error("Error en línea " + std::to_string(cur.line) + ": Se esperaba tipo después de 'new [exp]'");
        }
        advance();
        return;
    }
    if (cur.type == TokenType::LPAREN) {
        advance();
        exp();
        expect(TokenType::RPAREN, "Se esperaba ')'");
        return;
    }
    throw std::runtime_error("Error en linea " + std::to_string(cur.line) + ": expresion invalida (token: " + cur.lexeme + ")");
}
