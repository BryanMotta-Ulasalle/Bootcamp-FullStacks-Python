// mini0_utils.c

#include "mini0.h"
#include <errno.h> // Para manejo de errores de strtol

// Variables globales definidas aquí (declaradas como extern en mini0.h)
Token *tokens_list = NULL;
int tokens_count = 0;

// Implementación de traducir_escapes
char *traducir_escapes(const char *str) {
    size_t len = strlen(str);
    char *result = (char *)malloc(len + 1);
    int j = 0;
    for (int i = 1; i < len - 1; i++) {
        if (str[i] == '\\') {
            i++;
            switch (str[i]) {
                case 'n':  result[j++] = '\n'; break;
                case 't':  result[j++] = '\t'; break;
                case '\\': result[j++] = '\\'; break;
                case '\"': result[j++] = '\"'; break;
                default:
                    // Si el escape no es reconocido
                    result[j++] = '\\';
                    result[j++] = str[i];
            }
        } else {
            result[j++] = str[i];
        }
    }
    result[j] = '\0';
    return result;
}

// Implementación de crear_token
void crear_token(const char *tipo, const char *valor_str, long valor_num) {
    tokens_list = (Token *)realloc(tokens_list, (tokens_count + 1) * sizeof(Token));
    if (tokens_list == NULL) {
        perror("Error de realloc en tokens_list");
        exit(EXIT_FAILURE);
    }
    Token *t = &tokens_list[tokens_count];

    t->tipo = strdup(tipo);
    t->valor_str = strdup(valor_str);
    t->valor_num = valor_num;
    t->linea = yylineno;

    // Manejo de LITSTRING
    if (strcmp(tipo, "LITSTRING") == 0) {
        free(t->valor_str);
        t->valor_str = traducir_escapes(valor_str);
    }

    tokens_count++;
}

// Implementación de manejo de error
void token_error(const char *valor) {
    crear_token("ERROR", valor, 0);
    fprintf(stderr, ">>> ERROR LÉXICO en línea %d: Carácter o secuencia no reconocida: '%s'\n", yylineno, valor);
}

// Implementación de la lista de tokens y limpieza
void listar_tokens() {
    printf("\n--- Listado de Tokens ---\n");
    printf("%-5s | %-15s | %-15s | %-15s | %s\n", "Línea", "Tipo", "Valor Original", "Valor Numérico", "Valor String Traducido/ID");
    printf("------------------------------------------------------------------------------------------\n");

    for (int i = 0; i < tokens_count; i++) {
        Token t = tokens_list[i];
        char num_str[20];
        
        if (strcmp(t.tipo, "LITNUMERAL") == 0 || strcmp(t.tipo, "TRUE_LIT") == 0 || strcmp(t.tipo, "FALSE_LIT") == 0) {
             sprintf(num_str, "%ld", t.valor_num);
        } else {
             strcpy(num_str, "-");
        }

        // El valor_str para LITSTRING se muestra como [CADENA_RAW] en la columna de valor original
        const char *valor_original_display = (strcmp(t.tipo, "LITSTRING") == 0) ? "[CADENA_RAW]" : t.valor_str;
        
        printf("%-5d | %-15s | %-15s | %-15s | %s\n",
               t.linea, t.tipo, valor_original_display, num_str, t.valor_str);

        // Limpieza de memoria para el token actual
        free(t.tipo);
        free(t.valor_str);
    }
    free(tokens_list);
}

// Función principal
int main(int argc, char **argv) {
    extern FILE *yyin; // Declaración de la variable yyin de Flex

    if (argc != 2) {
        fprintf(stderr, "Uso: %s <archivo_mini0>\n", argv[0]);
        return 1;
    }

    yyin = fopen(argv[1], "r");
    if (!yyin) {
        perror("Error al abrir el archivo de entrada");
        return 1;
    }

    // Ejecutar el análisis léxico
    yylex();

    // Listar los tokens generados
    listar_tokens();

    fclose(yyin);
    return 0;
}