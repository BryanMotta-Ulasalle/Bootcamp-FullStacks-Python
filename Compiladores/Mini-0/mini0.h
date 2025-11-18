// mini0.h

#ifndef MINI0_H
#define MINI0_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Definición de la estructura del Token
typedef struct {
    char *tipo;        // Tipo de token (ej: ID, LITNUMERAL, IF, PLUS)
    char *valor_str;   // Valor como cadena (texto original o traducido)
    long valor_num;    // Valor numérico (para LITNUMERAL y booleanos)
    int linea;         // Número de línea
} Token;

// Variable externa para el número de línea (definida por Flex)
extern int yylineno;

// Lista global de tokens generados (definida en mini0_utils.c)
extern Token *tokens_list;
extern int tokens_count;

// Declaraciones de funciones auxiliares
void crear_token(const char *tipo, const char *valor_str, long valor_num);
void token_error(const char *valor);
char *traducir_escapes(const char *str);
void listar_tokens();

// Declaración de la función principal del lexer, generada por Flex
int yylex(void); 

#endif // MINI0_H