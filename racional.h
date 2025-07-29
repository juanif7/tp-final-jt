//racional.h
#ifndef RACIONAL_H
#define RACIONAL_H

#include <stdbool.h>
#include "entero.h"

typedef struct racional racional_t;


//lleva al racional a su minima expresion
void simplificar_racional(racional_t *r);

//crea una nueva estructura racional_t
racional_t *racional_crear(bool es_negativo, entero_t *numerador, entero_t *denominador);

//destruye la estructura racional_t
void racional_destruir(racional_t *q);

//devuelve el numerador del racional
const entero_t *racional_numerador(const racional_t *q);

//devuelve el denominador del racional
const entero_t *racional_denominador(const racional_t *q);

//indica si el racional posee signo negativo
bool racional_es_negativo(const racional_t *q);

//imprime por consola el racional
bool racional_imprimir(const racional_t *q);

//suma dos numeros racionales y lo guarda en q
racional_t *racional_sumar(const racional_t *q, const racional_t *r);

//resta dos nuneros racionales y lo guarda en q
racional_t *racional_restar(const racional_t *q, const racional_t *r);

//multipica dos numeros racionales y los guarda en q
racional_t *racional_multiplicar(const racional_t *q, const racional_t *r);

//divide dos numeros racionales y los guarda en q
racional_t *racional_dividir(const racional_t *q, const racional_t *r);

//devuelve el valor absoluto del racional
racional_t *racional_modulo(racional_t r);

//devuelve el producto factorial del racional
racional_t *racional_factorial(const racional_t *r);

//devuelve el inverso multiplicativo del racional
racional_t *racional_inverso(const racional_t *r);

#endif
