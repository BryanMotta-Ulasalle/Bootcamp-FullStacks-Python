

%{
#include "mini0.h"

// La función yylex() será generada por Flex, y el main está en mini0_utils.c
%}

%option noyywrap

%%
 /* ---------------------------------------------------- */
 /* REGLAS LÉXICAS                       */
 /* ---------------------------------------------------- */

[\r\n]+             { yylineno += yyleng; }
[ \t]+              { /* Ignorar espacios en blanco */ }
"//".* { /* Ignorar comentario de una línea */ }
"/*"([^*]|\*+[^*/])*\*+"/" { /* Ignorar comentario multi-línea */ }

 /* Palabras Reservadas */
"if"                { crear_token("IF", yytext, 0); }
"else"              { crear_token("ELSE", yytext, 0); }
"end"               { crear_token("END", yytext, 0); }
"while"             { crear_token("WHILE", yytext, 0); }
"loop"              { crear_token("LOOP", yytext, 0); }
"fun"               { crear_token("FUN", yytext, 0); }
"return"            { crear_token("RETURN", yytext, 0); }
"new"               { crear_token("NEW", yytext, 0); }
"string"            { crear_token("STRING_KW", yytext, 0); }
"int"               { crear_token("INT_KW", yytext, 0); }
"char"              { crear_token("CHAR_KW", yytext, 0); }
"bool"              { crear_token("BOOL_KW", yytext, 0); }
"true"              { crear_token("TRUE_LIT", yytext, 1); }
"false"             { crear_token("FALSE_LIT", yytext, 0); }
"and"               { crear_token("AND", yytext, 0); }
"or"                { crear_token("OR", yytext, 0); }
"not"               { crear_token("NOT", yytext, 0); }

 /* Literales Numerales */
[0-9]+              { crear_token("LITNUMERAL", yytext, strtol(yytext, NULL, 10)); }
"0x"[0-9a-fA-F]+    { crear_token("LITNUMERAL", yytext, strtol(yytext, NULL, 16)); }

 /* Literales String */
\"([^\\\n\"]|\\n|\\t|\\\\|\\\")*\" {
    crear_token("LITSTRING", yytext, 0);
}

 /* Identificadores */
[a-zA-Z_][a-zA-Z0-9_]* { crear_token("ID", yytext, 0); }

 /* Operadores y Puntuación (Longitud 2) */
"<="                { crear_token("LE", yytext, 0); }
">="                { crear_token("GE", yytext, 0); }
"<>"                { crear_token("NE", yytext, 0); }

 /* Operadores y Puntuación (Longitud 1) */
"("                 { crear_token("LPAREN", yytext, 0); }
")"                 { crear_token("RPAREN", yytext, 0); }
","                 { crear_token("COMMA", yytext, 0); }
":"                 { crear_token("COLON", yytext, 0); }
"<"                 { crear_token("LT", yytext, 0); }
">"                 { crear_token("GT", yytext, 0); }
"="                 { crear_token("ASSIGN", yytext, 0); }
"["                 { crear_token("LBRACKET", yytext, 0); }
"]"                 { crear_token("RBRACKET", yytext, 0); }
"+"                 { crear_token("PLUS", yytext, 0); }
"-"                 { crear_token("MINUS", yytext, 0); }
"*"                 { crear_token("TIMES", yytext, 0); }
"/"                 { crear_token("DIV", yytext, 0); }

 /* Error */
.                   { token_error(yytext); }

%%

// La función yylex() es generada por Flex aquí. No se necesita más código C.