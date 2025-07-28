#include "racional.h"
#include "entero.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

struct racional {
    // Representa una fracción mixta de la forma (-1)^s * (n / d).
    // s es el signo, n el numerador y d el denominador.
    // Invariante de representación:
    // *) n y d diferentes de NULL.
    // *) s será false o true según el número sea positivo o negativo respetivamente.
    // *) La fracción es irreducible, es decir n y d son coprimos.
    // *) d >= 1 (si n != 0).
    // *) Si n == 0 => d == 1 && s == false.
    bool s;
    entero_t *n, *d;
};

static entero_t *maximo_comun_denominador(const entero_t *a, const entero_t *b) {
   entero_t *x = entero_clonar(a);
   entero_t *y = entero_clonar(b);
   entero_t *resto;
   entero_t *cero = entero_cero();
    while(entero_comparar(y,cero)!= 0){
        entero_dividir(x,y,&resto);
        if(entero_comparar(resto,cero) == 0){ 
            entero_destruir(x);
            entero_destruir(cero);
            entero_destruir(resto);
            return y;
        }
        entero_destruir(x);
        x = y;
        y = resto;
   }
   entero_destruir(cero);
   return y;
}
static entero_t *minimo_comun_multiplo(const entero_t *a, const entero_t *b){ //mcm(a, b) = a*b/mcd(a,b)
    entero_t *mcd = maximo_comun_denominador(a, b);
    //calculo a*b
    entero_t *producto = entero_clonar(a);
    entero_multiplicar(producto, b);
    // Calcular MCM = (a*b)/MCD(a,b)
    entero_t *resultado = entero_clonar(producto);
    entero_dividir(resultado, mcd, NULL);
    // libero memoria intermedia
    entero_destruir(mcd);
    entero_destruir(producto);
    return resultado;
}
//nueva
void simplificar_racional(racional_t *r) {
    if(r == NULL) return;
    entero_t *cero = entero_cero();
    if(cero == NULL) return;
    if(entero_comparar(r->n, cero) == 0) {
        entero_destruir(r->d);
        r->d = entero_uno();
        r->s = false;
        entero_destruir(cero);
        return;
    }
    entero_destruir(cero);
    entero_t *mcd = maximo_comun_denominador(r->n, r->d);
    if(mcd == NULL) return;
    entero_t *uno = entero_uno();
    if(uno == NULL) {
        entero_destruir(mcd);
        return;
    }
    if(entero_comparar(mcd, uno) != 0) {
        entero_t *n = entero_clonar(r->n);
        entero_t *d = entero_clonar(r->d);
        if(n && d) {
            entero_dividir(n, mcd, NULL);
            entero_dividir(d, mcd, NULL);
            entero_destruir(r->n);
            entero_destruir(r->d);
            r->n = n;
            r->d = d;
        } else {
            if(n) entero_destruir(n);
            if(d) entero_destruir(d);
        }
    }
    entero_destruir(mcd);
    entero_destruir(uno);
}

racional_t *racional_crear(bool es_negativo, entero_t *numerador, entero_t *denominador){
    racional_t *r = malloc(sizeof(racional_t));
    if(r == NULL) return NULL;
    if(numerador == NULL){
        racional_destruir(r);
        return NULL;
    }
    if(denominador == NULL){
        r->d = entero_uno();
    }
    else{
        r->d = entero_clonar(denominador);
    }
    
    r->n = entero_clonar(numerador);
    r->s = es_negativo;
    simplificar_racional(r);
    return r;
}

void racional_destruir(racional_t *q){
    if(q->n)entero_destruir(q->n);//destruye q y n si estos existen.
    if(q->d)entero_destruir(q->d);
    free(q);
}

const entero_t *racional_denominador(const racional_t *q){
    return q-> d;
}

const entero_t *racional_numerador(const racional_t *q){
    return q-> n;
}

bool racional_es_negativo(const racional_t *q){
    return q->s;
}

bool racional_imprimir(const racional_t *q) {
    printf(q->s ? "-" : "+");
    if (!entero_imprimir(q->n)) return false;
    printf("/");
    if (!entero_imprimir(q->d)) return false;
    printf("\n");
    return true;
}
 
racional_t *racional_sumar(const racional_t *q, const racional_t *r){ //podria achicar lineas en el caso de que los signos sean iguales
    entero_t *mcm = minimo_comun_multiplo(q->d, r->d);
    entero_t *factorq = entero_clonar(mcm);
    entero_t *factorr = entero_clonar(mcm);
    entero_t *resto = NULL;
    entero_dividir(factorq, q->d, &resto);
    entero_dividir(factorr, r->d, &resto);
    entero_multiplicar(factorq, q->n);
    entero_multiplicar(factorr, r->n);

    entero_t *nuevo_numerador;
    bool es_negativo;

    if(q->s == r->s){ // mismo signo
        entero_sumar(factorq, factorr);
        es_negativo = q->s;
        nuevo_numerador = entero_clonar(factorq);
    }
    if(q->s != r->s){ //signos distintos
        int cmp = entero_comparar(factorq, factorr);
        if (cmp >= 0) {
            entero_restar(factorq, factorr);
            nuevo_numerador = entero_clonar(factorq);
            es_negativo = q->s;  // el mayor determina el signo del resultado
        } 
        else {
            entero_restar(factorr, factorq);
            nuevo_numerador = entero_clonar(factorr);
            es_negativo = r->s;  // el mayor determina el signo del resultado
        }
    }
    entero_destruir(factorq);
    entero_destruir(factorr);

    entero_t *cero = entero_cero();
    entero_t *uno = entero_uno();
    if(entero_comparar(nuevo_numerador, cero) == 0){
        entero_destruir(nuevo_numerador);
        entero_destruir(mcm);
        return racional_crear(false, cero, uno);
    }
    entero_destruir(cero);
    entero_destruir(uno);
    
    racional_t *resultado = racional_crear(es_negativo, nuevo_numerador, mcm);
    simplificar_racional(resultado);
    return resultado;
}

racional_t *racional_restar(const racional_t *q, const racional_t *r){
    //q + r = q + (-r) gracias roby!
    racional_t *r_aux = racional_crear(!(r->s), entero_clonar(r->n), entero_clonar(r->d));
    racional_t *resta = racional_sumar(q,r_aux);
    racional_destruir(r_aux);
    return resta;
}

racional_t *racional_multiplicar(const racional_t *q, const racional_t *r){
    //chequeo q nungún denominador sea cero.
    entero_t *cero = entero_cero();
    if (cero == NULL) return NULL;
    if (entero_comparar(q->d, cero) == 0 || entero_comparar(r->d, cero) == 0) {
        entero_destruir(cero);
        return NULL;
    }

    if(entero_comparar(q->n, cero) == 0|| entero_comparar(r->n, cero) == 0){
        return racional_crear(false, cero, entero_uno());
    }

    entero_t *numerador = entero_clonar(q->n);
    entero_t *denominador = entero_clonar(q->d);
    if (!numerador || !denominador) {
        if (numerador) entero_destruir(numerador);
        if (denominador) entero_destruir(denominador);
        return NULL;
    }

    entero_multiplicar(numerador, r->n);
    entero_multiplicar(denominador, r->d);

    bool signo;
    if(q->s == r->s){
        signo = false;
    }
    else signo = true;

    racional_t *resultado = racional_crear(signo, numerador, denominador);
    simplificar_racional(resultado);
    entero_destruir(numerador);
    entero_destruir(denominador);
    entero_destruir(cero);
    return resultado;

}

racional_t *racional_dividir(const racional_t *q, const racional_t *r) {
    if (q == NULL || r == NULL) return NULL;
    entero_t *cero = entero_cero();
    if (cero == NULL) return NULL;

    if (entero_comparar(r->n, cero) == 0) {
        entero_destruir(cero);
        return NULL;
    }
    entero_destruir(cero);

    racional_t *r_aux = racional_crear(r->s, r->d, r->n);
    if (r_aux == NULL) return NULL;

    racional_t *division = racional_multiplicar(q, r_aux);

    racional_destruir(r_aux);
    
    return division;
}

racional_t *racional_modulo(racional_t r){
    racional_t resultado = racional_crear(false, r->n, r->s);
    return resultado;
}

racional_t *racional_factorial(const racional_t *r) {
    if (r == NULL) {
        entero_t *cero = crear_entero_desde_numero(0);
        entero_t *uno = crear_entero_desde_numero(1);
        racional_t *error = racional_crear(false, cero, uno);
        entero_destruir(cero);
        entero_destruir(uno);
        return error;
    }
    
    //no hay factorial negativo
    if (racional_es_negativo(r)) {
        // Factorial no definido para números negativos
        entero_t *cero = crear_entero_desde_numero(0);
        entero_t *uno = crear_entero_desde_numero(1);
        racional_t *error = racional_crear(false, cero, uno);
        entero_destruir(cero);
        entero_destruir(uno);
        return error;
    }
    
    //solo se puede hacer factorial de enteros (numerador/1)
    const entero_t *denominador = racional_denominador(r);
    entero_t *uno = crear_entero_desde_numero(1);
    
    if (entero_comparar(denominador, uno) != 0) {
        // No es un entero (denominador ≠ 1), factorial no definido
        entero_t *cero = crear_entero_desde_numero(0);
        racional_t *error = racional_crear(false, cero, uno);
        entero_destruir(cero);
        entero_destruir(uno);
        return error;
    }
    
    // Obtener el numerador y calcular su factorial
    const entero_t *numerador = racional_numerador(r);
    entero_t *factorial_resultado = entero_factorial(numerador);
    
    // Crear el racional resultado: factorial_resultado/1
    entero_t *denominador_resultado = entero_clonar(uno);
    racional_t *resultado = racional_crear(false, factorial_resultado, denominador_resultado);
    
    // Limpiar memoria temporal
    entero_destruir(factorial_resultado);
    entero_destruir(denominador_resultado);
    entero_destruir(uno);
    
    return resultado;
}