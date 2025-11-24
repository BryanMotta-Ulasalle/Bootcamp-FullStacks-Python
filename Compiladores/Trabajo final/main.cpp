#include "lexer.h"
#include "parser.h"
#include <iostream>
#include <fstream>

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Uso: mini0 <archivo>\n";
        return 1;
    }

    std::ifstream f(argv[1]);
    if (!f) {
        std::cerr << "No se pudo abrir el archivo.\n";
        return 1;
    }

    try {
        Lexer lex(f);
        Parser parser(lex);
        parser.parse();

        std::cout << "OK\n";
        return 0;
    }
    catch (const std::exception &e) {
        std::cerr << e.what() << "\n";
        return 1;
    }
}
