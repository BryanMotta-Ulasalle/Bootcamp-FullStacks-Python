#include "lexer.h"
#include <cctype>
#include <stdexcept>

Lexer::Lexer(std::ifstream &in) {
    src.assign((std::istreambuf_iterator<char>(in)),
                std::istreambuf_iterator<char>());
}

bool Lexer::eof() const { return pos >= src.size(); }
char Lexer::current() const { return eof() ? '\0' : src[pos]; }
char Lexer::advance() { return eof() ? '\0' : src[pos++]; }

bool Lexer::match(char expected) {
    if (current() == expected) { pos++; return true; }
    return false;
}

void Lexer::skipSpaces() {
    while (!eof()) {
        if (current() == '\n') return;
        if (isspace(current())) advance();
        else break;
    }
}

Token Lexer::peek() {
    size_t savePos = pos;
    int saveLine = line;
    Token t = next();
    pos = savePos;
    line = saveLine;
    return t;
}

Token Lexer::idOrKeyword(const std::string& word) {
    static std::unordered_map<std::string, TokenType> keywords = {
        {"fun", TokenType::FUN}, {"end", TokenType::END}, {"else", TokenType::ELSE},
        {"if", TokenType::IF}, {"while", TokenType::WHILE}, {"loop", TokenType::LOOP},
        {"return", TokenType::RETURN},
        {"true", TokenType::TRUE_}, {"false", TokenType::FALSE_},
        {"new", TokenType::NEW_}, {"and", TokenType::AND_}, {"or", TokenType::OR_},
        {"not", TokenType::NOT_},
        {"int", TokenType::INT}, {"bool", TokenType::BOOL},
        {"char", TokenType::CHAR}, {"string", TokenType::STRING}
    };

    if (keywords.count(word)) return Token(keywords[word], word, line);
    return Token(TokenType::ID, word, line);
}

Token Lexer::identifier() {
    std::string lex;
    while (isalnum(current()) || current() == '_')
        lex += advance();
    return idOrKeyword(lex);
}

Token Lexer::number() {
    std::string lex;
    while (isdigit(current()))
        lex += advance();
    return Token(TokenType::LITNUMERAL, lex, line);
}

Token Lexer::stringLiteral() {
    std::string lex;
    advance(); // skip "
    while (!eof() && current() != '"') {
        lex += advance();
    }
    if (eof()) throw std::runtime_error("String no cerrado en línea " + std::to_string(line));
    advance(); // skip final "
    return Token(TokenType::LITSTRING, lex, line);
}

Token Lexer::next() {
    skipSpaces();

    if (eof()) return Token(TokenType::END_OF_FILE, "", line);

    if (match('\n')) {
        line++;
        return Token(TokenType::NL, "\\n", line);
    }

    char c = advance();

    // Símbolos
    switch (c) {
        case '(' : return Token(TokenType::LPAREN, "(", line);
        case ')' : return Token(TokenType::RPAREN, ")", line);
        case '[' : return Token(TokenType::LBRACKET, "[", line);
        case ']' : return Token(TokenType::RBRACKET, "]", line);
        case ':' : return Token(TokenType::COLON, ":", line);
        case ',' : return Token(TokenType::COMMA, ",", line);
        case '=' : return match('=') ? Token(TokenType::EQ, "==", line)
                                    : Token(TokenType::ASSIGN, "=", line);
        case '<' :
            if (match('=')) return Token(TokenType::LE, "<=", line);
            if (match('>')) return Token(TokenType::NE, "<>", line);
            return Token(TokenType::LT, "<", line);
        case '>' : return match('=') ? Token(TokenType::GE, ">=", line)
                                    : Token(TokenType::GT, ">", line);
        case '+' : return Token(TokenType::PLUS, "+", line);
        case '-' : return Token(TokenType::MINUS, "-", line);
        case '*' : return Token(TokenType::STAR, "*", line);
        case '/' : return Token(TokenType::SLASH, "/", line);
        case '"' : return stringLiteral();
    }

    // Identificador
    if (isalpha(c) || c == '_') {
        pos--;
        return identifier();
    }

    // Número
    if (isdigit(c)) {
        pos--;
        return number();
    }

    return Token(TokenType::INVALID, std::string(1,c), line);
}
