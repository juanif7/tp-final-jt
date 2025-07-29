#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "cola.h"

// Enumeración para tipos de tokens
typedef enum {
    TOKEN_NUMERO,
    TOKEN_OPERADOR_BINARIO,    // +, -, x, /, ^
    TOKEN_OPERADOR_UNARIO,     // _ (cambio de signo)
    TOKEN_FUNCION,             // fact, abs, sen, cos, tg
    TOKEN_CONSTANTE,           // pi, e, phi
    TOKEN_PARENTESIS,          // (, )
    TOKEN_INVALIDO
} TipoToken;

// Estructura para representar un token
typedef struct {
    TipoToken tipo;
    char *valor;
} Token;

// Función para crear un token
Token *crear_token(TipoToken tipo, const char *valor) {
    Token *token = malloc(sizeof(Token));
    if (!token) return NULL;
    
    token->tipo = tipo;
    token->valor = malloc(strlen(valor) + 1);
    if (!token->valor) {
        free(token);
        return NULL;
    }
    strcpy(token->valor, valor);
    return token;
}

// Función para destruir un token
void destruir_token(void *token_ptr) {
    if (!token_ptr) return;
    Token *token = (Token *)token_ptr;
    free(token->valor);
    free(token);
}

// Función para verificar si una cadena es un operador binario
bool es_operador_binario(const char *str) {
    return (strcmp(str, "+") == 0 || strcmp(str, "-") == 0 || 
            strcmp(str, "x") == 0 || strcmp(str, "/") == 0 || 
            strcmp(str, "^") == 0);
}

// Función para verificar si una cadena es una función
bool es_funcion(const char *str) {
    return (strcmp(str, "fact") == 0 || strcmp(str, "abs") == 0 ||
            strcmp(str, "sen") == 0 || strcmp(str, "cos") == 0 ||
            strcmp(str, "tg") == 0);
}

// Función para verificar si una cadena es una constante
bool es_constante(const char *str) {
    return (strcmp(str, "pi") == 0 || strcmp(str, "e") == 0 ||
            strcmp(str, "phi") == 0);
}

// Función para verificar si un carácter puede ser parte de un número
bool es_caracter_numero(char c) {
    return (isdigit(c) || c == '.');
}

// Función para verificar si una cadena es un número válido
bool es_numero_valido(const char *str) {
    if (!str || strlen(str) == 0) return false;
    
    bool punto_encontrado = false;
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '.') {
            if (punto_encontrado) return false; // Más de un punto
            punto_encontrado = true;
        } else if (!isdigit(str[i])) {
            return false;
        }
    }
    return true;
}

// Función para leer entrada del usuario con memoria dinámica
char *leer_entrada_usuario() {
    printf("Ingresa la expresión matemática: ");
    
    size_t buffer_size = 64;
    char *entrada = malloc(buffer_size);
    if (!entrada) {
        printf("Error: No se pudo asignar memoria\n");
        return NULL;
    }
    
    size_t len = 0;
    int c;
    
    // Leer carácter por carácter
    while ((c = getchar()) != '\n' && c != EOF) {
        // Expandir buffer si es necesario
        if (len >= buffer_size - 1) {
            buffer_size *= 2;
            char *temp = realloc(entrada, buffer_size);
            if (!temp) {
                printf("Error: No se pudo expandir la memoria\n");
                free(entrada);
                return NULL;
            }
            entrada = temp;
        }
        entrada[len++] = c;
    }
    
    entrada[len] = '\0';
    return entrada;
}

// Función para extraer una palabra/token desde una posición
char *extraer_palabra(const char *entrada, int inicio, int fin) {
    int longitud = fin - inicio;
    char *palabra = malloc(longitud + 1);
    if (!palabra) return NULL;
    
    strncpy(palabra, entrada + inicio, longitud);
    palabra[longitud] = '\0';
    return palabra;
}

// Función para limpiar memoria en caso de error
void limpiar_cola_error(cola_t *cola_tokens) {
    if (cola_tokens) {
        cola_destruir(cola_tokens, destruir_token);
    }
}

// Función para procesar un número
bool procesar_numero(cola_t *cola_tokens, const char *entrada, int *i, int len) {
    int inicio = *i;
    
    // Avanzar mientras sea parte del número
    while (*i < len && es_caracter_numero(entrada[*i])) {
        (*i)++;
    }
    
    char *numero_str = extraer_palabra(entrada, inicio, *i);
    if (!numero_str) return false;
    
    // Validar que sea un número válido
    if (!es_numero_valido(numero_str)) {
        printf("Error: Número inválido '%s'\n", numero_str);
        free(numero_str);
        return false;
    }
    
    Token *token = crear_token(TOKEN_NUMERO, numero_str);
    free(numero_str);
    
    if (!token || !cola_encolar(cola_tokens, token)) {
        if (token) destruir_token(token);
        return false;
    }
    
    return true;
}

// Función para procesar operador unario _
bool procesar_operador_unario(cola_t *cola_tokens, int *i) {
    Token *token = crear_token(TOKEN_OPERADOR_UNARIO, "_");
    if (!token || !cola_encolar(cola_tokens, token)) {
        if (token) destruir_token(token);
        return false;
    }
    (*i)++;
    return true;
}

// Función para procesar operadores binarios
bool procesar_operador_binario(cola_t *cola_tokens, char operador, int *i) {
    char op_str[2] = {operador, '\0'};
    Token *token = crear_token(TOKEN_OPERADOR_BINARIO, op_str);
    if (!token || !cola_encolar(cola_tokens, token)) {
        if (token) destruir_token(token);
        return false;
    }
    (*i)++;
    return true;
}

// Función para procesar paréntesis
bool procesar_parentesis(cola_t *cola_tokens, char parentesis, int *i) {
    char paren_str[2] = {parentesis, '\0'};
    Token *token = crear_token(TOKEN_PARENTESIS, paren_str);
    if (!token || !cola_encolar(cola_tokens, token)) {
        if (token) destruir_token(token);
        return false;
    }
    (*i)++;
    return true;
}

// Función para procesar palabras (funciones y constantes)
bool procesar_palabra(cola_t *cola_tokens, const char *entrada, int *i, int len) {
    int inicio = *i;
    
    // Avanzar mientras sea letra
    while (*i < len && isalpha(entrada[*i])) {
        (*i)++;
    }
    
    char *palabra = extraer_palabra(entrada, inicio, *i);
    if (!palabra) return false;
    
    Token *token = NULL;
    
    // Determinar el tipo de token
    if (es_funcion(palabra)) {
        token = crear_token(TOKEN_FUNCION, palabra);
    } else if (es_constante(palabra)) {
        token = crear_token(TOKEN_CONSTANTE, palabra);
    } else {
        printf("Error: Símbolo desconocido '%s'\n", palabra);
        free(palabra);
        return false;
    }
    
    free(palabra);
    
    if (!token || !cola_encolar(cola_tokens, token)) {
        if (token) destruir_token(token);
        return false;
    }
    
    return true;
}

// Función principal para parsear la entrada y crear la cola de tokens
cola_t *parsear_entrada(const char *entrada) {
    if (!entrada) return NULL;
    
    cola_t *cola_tokens = cola_crear();
    if (!cola_tokens) return NULL;
    
    int len = strlen(entrada);
    int i = 0;
    bool error_ocurrido = false;
    
    while (i < len && !error_ocurrido) {
        // Saltar espacios en blanco
        if (isspace(entrada[i])) {
            i++;
            continue;
        }
        
        // Procesar números (incluyendo decimales)
        if (isdigit(entrada[i]) || entrada[i] == '.') {
            if (!procesar_numero(cola_tokens, entrada, &i, len)) {
                error_ocurrido = true;
            }
        }
        // Procesar operador unario _ (cambio de signo)
        else if (entrada[i] == '_') {
            if (!procesar_operador_unario(cola_tokens, &i)) {
                error_ocurrido = true;
            }
        }
        // Procesar operadores de un carácter (+, -, x, /, ^)
        else if (entrada[i] == '+' || entrada[i] == '-' || entrada[i] == 'x' || 
                 entrada[i] == '/' || entrada[i] == '^') {
            if (!procesar_operador_binario(cola_tokens, entrada[i], &i)) {
                error_ocurrido = true;
            }
        }
        // Procesar paréntesis
        else if (entrada[i] == '(' || entrada[i] == ')') {
            if (!procesar_parentesis(cola_tokens, entrada[i], &i)) {
                error_ocurrido = true;
            }
        }
        // Procesar palabras (funciones y constantes)
        else if (isalpha(entrada[i])) {
            if (!procesar_palabra(cola_tokens, entrada, &i, len)) {
                error_ocurrido = true;
            }
        }
        // Carácter no reconocido
        else {
            printf("Error: Carácter no válido '%c' en posición %d\n", entrada[i], i);
            error_ocurrido = true;
        }
    }
    
    if (error_ocurrido) {
        limpiar_cola_error(cola_tokens);
        return NULL;
    }
    
    return cola_tokens;
}

// Función para obtener el nombre del tipo de token
const char *nombre_tipo_token(TipoToken tipo) {
    if (tipo == TOKEN_NUMERO) return "NÚMERO";
    if (tipo == TOKEN_OPERADOR_BINARIO) return "OPERADOR BINARIO";
    if (tipo == TOKEN_OPERADOR_UNARIO) return "OPERADOR UNARIO";
    if (tipo == TOKEN_FUNCION) return "FUNCIÓN";
    if (tipo == TOKEN_CONSTANTE) return "CONSTANTE";
    if (tipo == TOKEN_PARENTESIS) return "PARÉNTESIS";
    if (tipo == TOKEN_INVALIDO) return "INVÁLIDO";
    return "DESCONOCIDO";
}

// Función para mostrar los tokens de la cola (sin destruirla)
void mostrar_tokens(cola_t *cola_tokens) {
    if (!cola_tokens || cola_esta_vacia(cola_tokens)) {
        printf("No hay tokens para mostrar.\n");
        return;
    }
    
    printf("\n=== TOKENS IDENTIFICADOS ===\n");
    
    // Crear una cola temporal para no modificar la original
    cola_t *cola_temp = cola_crear();
    if (!cola_temp) return;
    
    int contador = 1;
    
    // Procesar cada token
    while (!cola_esta_vacia(cola_tokens)) {
        Token *token = (Token *)cola_desencolar(cola_tokens);
        
        printf("Token %d: %s -> '%s'\n", contador++, 
               nombre_tipo_token(token->tipo), token->valor);
        
        // Guardar en cola temporal
        cola_encolar(cola_temp, token);
    }
    
    // Restaurar la cola original
    while (!cola_esta_vacia(cola_temp)) {
        Token *token = (Token *)cola_desencolar(cola_temp);
        cola_encolar(cola_tokens, token);
    }
    
    cola_destruir(cola_temp, NULL);
}

// Función para contar tokens por tipo
void contar_token_tipo(Token *token, int *contadores) {
    if (token->tipo == TOKEN_NUMERO) contadores[0]++;
    else if (token->tipo == TOKEN_OPERADOR_BINARIO) contadores[1]++;
    else if (token->tipo == TOKEN_OPERADOR_UNARIO) contadores[2]++;
    else if (token->tipo == TOKEN_FUNCION) contadores[3]++;
    else if (token->tipo == TOKEN_CONSTANTE) contadores[4]++;
    else if (token->tipo == TOKEN_PARENTESIS) contadores[5]++;
}

// Función para procesar y mostrar información adicional sobre tokens
void analizar_tokens(cola_t *cola_tokens) {
    if (!cola_tokens || cola_esta_vacia(cola_tokens)) return;
    
    printf("\n=== ANÁLISIS DE TOKENS ===\n");
    
    // Array para contadores: [numeros, op_bin, op_un, funciones, constantes, parentesis]
    int contadores[6] = {0, 0, 0, 0, 0, 0};
    
    // Crear cola temporal para análisis
    cola_t *cola_temp = cola_crear();
    if (!cola_temp) return;
    
    // Contar tipos de tokens
    while (!cola_esta_vacia(cola_tokens)) {
        Token *token = (Token *)cola_desencolar(cola_tokens);
        contar_token_tipo(token, contadores);
        cola_encolar(cola_temp, token);
    }
    
    // Restaurar cola original
    while (!cola_esta_vacia(cola_temp)) {
        Token *token = (Token *)cola_desencolar(cola_temp);
        cola_encolar(cola_tokens, token);
    }
    
    cola_destruir(cola_temp, NULL);
    
    // Mostrar estadísticas
    printf("Números: %d\n", contadores[0]);
    printf("Operadores binarios: %d\n", contadores[1]);
    printf("Operadores unarios: %d\n", contadores[2]);
    printf("Funciones: %d\n", contadores[3]);
    printf("Constantes: %d\n", contadores[4]);
    printf("Paréntesis: %d\n", contadores[5]);
}

// Función principal que integra todo el proceso
cola_t *procesar_entrada_usuario() {
    printf("=== PARSER AVANZADO DE CALCULADORA ===\n");
    printf("Operadores soportados:\n");
    printf("  Binarios: +, -, x, /, ^\n");
    printf("  Unarios: _ (cambio de signo)\n");
    printf("Funciones: fact, abs, sen, cos, tg\n");
    printf("Constantes: pi, e, phi\n");
    printf("Paréntesis: (, )\n");
    printf("Ejemplo: 2 x pi + sen(30) - _5 + fact(4)\n\n");
    
    // Leer entrada del usuario
    char *entrada = leer_entrada_usuario();
    if (!entrada) return NULL;
    
    printf("\nEntrada recibida: '%s'\n", entrada);
    
    // Parsear la entrada
    cola_t *cola_tokens = parsear_entrada(entrada);
    free(entrada);
    
    if (!cola_tokens) {
        printf("Error en el procesamiento de la entrada.\n");
        return NULL;
    }
    
    // Mostrar tokens identificados
    mostrar_tokens(cola_tokens);
    
    // Mostrar análisis
    analizar_tokens(cola_tokens);
    
    return cola_tokens;
}

// Función principal para demostración
int main() {
    cola_t *tokens = procesar_entrada_usuario();
    
    if (tokens) {
        printf("\n¡Parsing completado exitosamente!\n");
        printf("La cola contiene todos los tokens identificados.\n");
        
        // Aquí podrías continuar con el procesamiento de los tokens
        // Por ejemplo, convertir a postfija o evaluar la expresión
        
        // Limpiar memoria
        cola_destruir(tokens, destruir_token);
    }
    
    return 0;
}