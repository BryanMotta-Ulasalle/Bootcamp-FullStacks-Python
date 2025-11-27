#include "lexer.h"
#include "parser.h"
#include <iostream>
#include <fstream>

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Uso: mini0 <archivo.m0>\n";
        return 2;
    }
    std::ifstream in(argv[1]);
    if (!in) {
        std::cerr << "No se pudo abrir el archivo: " << argv[1] << "\n";
        return 2;
    }

    try {
        Lexer lex(in);
        Parser parser(lex);
        parser.programa(); // start parsing
        // ensure EOF consumed
        Token t = lex.next();
        if (t.type != TokenType::END_OF_FILE) {
            std::cerr << "Error: tokens sobrantes a partir de línea " << t.line << "\n";
            return 1;
        }
        std::cout << "ANALISIS OK\n";
        return 0;
    } catch (const std::runtime_error &e) {
        std::cerr << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "Error inesperado\n";
        return 1;
    }
}
