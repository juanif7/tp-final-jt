#ifndef ENTERO_H
#define ENTERO_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct entero entero_t;

// Crea un entero con n palabras de 32 bits inicializadas en 0
entero_t *_crear(size_t n);

// Destruye un entero liberando su memoria
void entero_destruir(entero_t *entero);

// Crea un clon del entero recibido
entero_t *entero_clonar(const entero_t *entero);

// Redimensiona un entero a n palabras (solo permite aumentar el tamaño)
bool _redimensionar(entero_t *entero, size_t n);

// Compara dos enteros: 1 si a > b, -1 si a < b, 0 si son iguales
int entero_comparar(const entero_t *a, const entero_t *b);

// Desplaza los bits del entero una posición a la derecha (división por 2)
bool entero_desplazar_derecha(entero_t *e);

// Desplaza los bits del entero una posición a la izquierda (multiplicación por 2)
bool entero_desplazar_izquierda(entero_t *e);

// Suma el entero b al entero a (a = a + b)
bool entero_sumar(entero_t *a, const entero_t *b);

// Resta el entero b al entero a (a = a - b, a debe ser ≥ b)
bool entero_restar(entero_t *a, const entero_t *b);

// Convierte un entero a su representación BCD (Binary Coded Decimal)
char *entero_a_bcd(const entero_t *entero, size_t *n);

// Crea un entero desde una representación BCD
entero_t *entero_desde_bcd(const char bcd[], size_t n);

// Divide dividendo por divisor, retorna cociente y guarda el resto en *resto
entero_t *entero_dividir(const entero_t *dividendo, const entero_t *divisor, entero_t **resto);

// Imprime un entero en formato decimal
bool entero_imprimir(const entero_t *entero);

// Lee un número en formato BCD desde la entrada estándar
char *leer_bcd(size_t *n);

entero_t *multiplicar_enteros_largos(const entero_t *x, const entero_t *y);

entero_t *entero_factorial(const entero_t *n);

entero_t *crear_entero_desde_int(int numero);

entero_t *entero_potencia(const entero_t *a, const entero_t *b);
#endif