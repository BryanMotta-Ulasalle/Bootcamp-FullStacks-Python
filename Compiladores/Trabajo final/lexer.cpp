#include "lexer.h"
#include <iterator>
#include <cctype>
#include <unordered_map>
#include <stdexcept>

Lexer::Lexer(std::ifstream &in) : pos(0), line(1) {
    src.assign((std::istreambuf_iterator<char>(in)),
                std::istreambuf_iterator<char>());
}

bool Lexer::eof() const { return pos >= src.size(); }
char Lexer::current() const { return eof() ? '\0' : src[pos]; }
char Lexer::advance() { return eof() ? '\0' : src[pos++]; }
bool Lexer::match(char expected) {
    if (!eof() && src[pos] == expected) { pos++; return true; }
    return false;
}

void Lexer::skipSpacesExceptNewline() {
    while (!eof()) {
        char c = current();
        if (c == '\r') { advance(); continue; } // ignore CR
        if (c == '\n') break;
        if (isspace(static_cast<unsigned char>(c))) { advance(); continue; }
        // comments
        if (c == '/' && pos + 1 < src.size()) {
            char n = src[pos+1];
            if (n == '/') {
                // line comment until newline
                advance(); advance();
                while (!eof() && current() != '\n') advance();
                continue;
            } else if (n == '*') {
                // block comment
                advance(); advance();
                while (!eof()) {
                    if (current() == '*' && pos + 1 < src.size() && src[pos+1] == '/') {
                        advance(); advance(); break;
                    } else {
                        if (current() == '\n') line++;
                        advance();
                    }
                }
                continue;
            }
        }
        break;
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

Token Lexer::identifierOrKeyword(const std::string &word) {
    static const std::unordered_map<std::string, TokenType> kw = {
        {"fun", TokenType::FUN}, {"end", TokenType::END}, {"else", TokenType::ELSE},
        {"if", TokenType::IF}, {"while", TokenType::WHILE}, {"loop", TokenType::LOOP},
        {"return", TokenType::RETURN},
        {"true", TokenType::TRUE_}, {"false", TokenType::FALSE_},
        {"new", TokenType::NEW_}, {"and", TokenType::AND_}, {"or", TokenType::OR_},
        {"not", TokenType::NOT_},
        {"int", TokenType::INT}, {"bool", TokenType::BOOL},
        {"char", TokenType::CHAR}, {"string", TokenType::STRING}
    };
    auto it = kw.find(word);
    if (it != kw.end()) return Token(it->second, word, line);
    return Token(TokenType::ID, word, line);
}

Token Lexer::identifier() {
    std::string lex;
    while (!eof() && (isalnum(static_cast<unsigned char>(current())) || current() == '_')) {
        lex.push_back(advance());
    }
    return identifierOrKeyword(lex);
}

Token Lexer::number() {
    std::string lex;
    while (!eof() && isdigit(static_cast<unsigned char>(current()))) lex.push_back(advance());
    return Token(TokenType::LITNUMERAL, lex, line);
}

Token Lexer::stringLiteral() {
    std::string lex;
    // current is opening quote '"', caller should have advanced it
    while (!eof() && current() != '"') {
        if (current() == '\\') {
            advance();
            if (eof()) break;
            char esc = advance();
            if (esc == 'n') lex.push_back('\n');
            else lex.push_back(esc);
        } else {
            lex.push_back(advance());
        }
    }
    if (eof()) throw std::runtime_error("Lexical error: string no cerrado en línea " + std::to_string(line));
    advance(); // consume closing "
    return Token(TokenType::LITSTRING, lex, line);
}

Token Lexer::next() {
    skipSpacesExceptNewline();

    if (eof()) return Token(TokenType::END_OF_FILE, "", line);

    // Handle newline as distinct token
    if (current() == '\n') {
        advance();
        line++;
        return Token(TokenType::NL, "\\n", line-1);
    }

    char c = advance();

    // Symbols and multi-char operators
    if (c == '(') return Token(TokenType::LPAREN, "(", line);
    if (c == ')') return Token(TokenType::RPAREN, ")", line);
    if (c == '[') return Token(TokenType::LBRACKET, "[", line);
    if (c == ']') return Token(TokenType::RBRACKET, "]", line);
    if (c == ':') return Token(TokenType::COLON, ":", line);
    if (c == ',') return Token(TokenType::COMMA, ",", line);
    if (c == '+') return Token(TokenType::PLUS, "+", line);
    if (c == '-') return Token(TokenType::MINUS, "-", line);
    if (c == '*') return Token(TokenType::STAR, "*", line);
    if (c == '/') return Token(TokenType::SLASH, "/", line);

    if (c == '=') {
        if (!eof() && current() == '=') { advance(); return Token(TokenType::EQ, "==", line); }
        return Token(TokenType::ASSIGN, "=", line);
    }
    if (c == '<') {
        if (!eof() && current() == '=') { advance(); return Token(TokenType::LE, "<=", line); }
        if (!eof() && current() == '>') { advance(); return Token(TokenType::NE, "<>", line); }
        return Token(TokenType::LT, "<", line);
    }
    if (c == '>') {
        if (!eof() && current() == '=') { advance(); return Token(TokenType::GE, ">=", line); }
        return Token(TokenType::GT, ">", line);
    }
    if (c == '"') {
        // string literal: current was '"', so call stringLiteral which expects closing "
        return stringLiteral();
    }

    // identifier or number
    if (isalpha(static_cast<unsigned char>(c)) || c == '_') {
        // roll back one position and call identifier()
        pos--;
        return identifier();
    }

    if (isdigit(static_cast<unsigned char>(c))) {
        pos--;
        return number();
    }

    // Unknown
    return Token(TokenType::INVALID, std::string(1,c), line);
}
