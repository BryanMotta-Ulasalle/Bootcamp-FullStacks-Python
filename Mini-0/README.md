# 🚀 Analizador Léxico (Lexer) de Mini-0
## 📝 Descripción del Proyecto
Este proyecto implementa el analizador léxico (Lexer) para el lenguaje de programación de juguete Mini-0, utilizando la herramienta Flex y el lenguaje C. Su función principal es leer un archivo fuente de Mini-0 y convertir la secuencia de caracteres en una lista de tokens (Token), que es la salida necesaria para la siguiente fase de compilación (el análisis sintáctico).

El código está dividido en tres archivos clave para una gestión limpia:

``mini0.h:`` El Contrato. Define la estructura Token y declara todas las funciones y variables.

``mini0_lexer.l:`` La Definición de Reglas. Contiene las expresiones regulares de Flex.

``mini0_utils.c:`` La Implementación. Contiene la función main, la lógica de gestión de memoria (realloc, strdup), y la traducción de strings.

#### Características Clave:
Manejo de Literales: Conversión de numerales decimales y hexadecimales al mismo valor numérico (strtol).

#### Traducción de Strings:
 Implementación de la traducción de secuencias de escape (\n, \t, etc.).

#### Reporte de Errores:
 Detección y registro de tokens inválidos (errores léxicos).

## 🛠️ Requisitos de Instalación
Para compilar y ejecutar este proyecto, necesitas tener instalados los siguientes componentes: Flex (o Lex) y GCC (GNU Compiler Collection).

### 🐧 Instalación en Linux (Ubuntu/Debian)
Bash:

``sudo apt update``

``sudo apt install flex build-essential``

#### 🍎 Instalación en macOS (usando Homebrew)
Bash:

``brew install flex gcc``

#### ⚙️ Uso y Compilación
Sigue estos tres pasos para generar el ejecutable y probar el analizador léxico.

1. Generar el Código C de Flex

Este comando lee el archivo de reglas (mini0_lexer.l) y genera el archivo lex.yy.c, que contiene el motor léxico (yylex).

Bash:

``flex mini0_lexer.l``

2. Compilar y Enlazar

Este comando compila el código generado (lex.yy.c) y las utilidades (mini0_utils.c), enlazándolos con la biblioteca de Flex (-lfl) para crear el ejecutable mini0_lexer.

Bash:

``gcc lex.yy.c mini0_utils.c -o mini0_lexer -lfl``

3. Ejecutar el Analizador

Ejecuta el programa pasándole el archivo fuente de Mini-0 (e.g., test_mini0.m0) como argumento.

Bash:

``./mini0_lexer test_mini0.m0``

El programa mostrará cualquier ERROR LÉXICO encontrado en la terminal y luego presentará una lista completa de los tokens generados.

#### 📌 Flujo de Ejecución (Alto Nivel)
#### Inicio:
La función main en mini0_utils.c abre el archivo y llama a yylex().

#### Análisis: 
yylex() (generada por Flex) lee la entrada y ejecuta acciones al coincidir con un patrón.

#### Registro:
 Las acciones llaman a crear_token (en mini0_utils.c) para guardar el token, manejando la memoria y la traducción de escapes.

#### Finalización:
 Al llegar a EOF, yylex() regresa a main. main llama a listar_tokens() para imprimir la salida y liberar la memoria asignada.
