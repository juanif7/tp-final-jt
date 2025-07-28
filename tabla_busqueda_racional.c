#include "racional.h"

typedef struct{
    char *nombre;   //"+", "-", etc
    int aridad;     // cantidad de operandos
    int prioridad;  //1, 2 o 3 
    racional_t *(*funcion)(const racional_t *, const racional_t *); //operacion asociada
    const char *descripcion; //que hace la funcion
}operacion_t;

typedef struct{
    char *nombre;
    int prioridad;
    int parametros;
    racional_t *(*funcion)(const racional_t *, const racional_t *);
    const char descripcion;
}

satatic const operacion_t OPERACIONES[]{
    {"+", 2, 1, racional_sumar, "a + b"},
    {"-", 2, 1, racional_restar, "a - b"},
    {"*", 2, 2, racional_multiplicar, "a * b"},
    {"/", 2, 3, racional_dividir, "a / b"},
    {"^", 2, 4, racional_potencia, "a ^ b"},
    {"_", 1, 5, racional_invertir, "-a"},
};

static const funcion_t FUNCIONES[]{
    {"fact", ??, 1, racional_factorial, "a!"},
    {"abs", ??, 1, racional_absoluto, "|a|"},
    {"inv", ??, 1, racional_inverso, "a^-1"},
    {"pi", ??, 0, ??, "3.1416"},
    {"e", ??, 0, ??, "2.7182"},
    {"phi", ??, 0, ??, "1.618"},
}

//crear las funciones asociadas a las operaciones tanto para racional como para entero
//verificar si hay que crear una tabla de busqueda para cada uno o si se puede usar la misma para los dos

//inchequeable que esto funcione pero que se yo queria hacer algo 