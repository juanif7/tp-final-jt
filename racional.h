//racional.h
#ifndef RACIONAL_H
#define RACIONAL_H

#include <stdbool.h>
#include "entero.h"

typedef struct racional racional_t;

racional_t *racional_crear(bool es_negativo, entero_t *numerador, entero_t *denominador);
void racional_destruir(racional_t *q);
const entero_t *racional_numerador(const racional_t *q);
const entero_t *racional_denominador(const racional_t *q);
bool racional_es_negativo(const racional_t *q);
bool racional_imprimir(const racional_t *q);
racional_t *racional_sumar(const racional_t *q, const racional_t *r);
racional_t *racional_restar(const racional_t *q, const racional_t *r);
racional_t *racional_multiplicar(const racional_t *q, const racional_t *r);
racional_t *racional_dividir(const racional_t *q, const racional_t *r);

#endif
