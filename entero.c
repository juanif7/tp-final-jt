// tp2
#include <stdio.h>
#include<string.h>
#include<stdint.h>
#include<stdbool.h>
#include<stdlib.h>
#include<math.h>

#define MASK_MSB 0x80000000//mascara con un 1 en el msb
#define MASK_LSB 0x1 //mascara con un 1 en el lsb
#define MASK_32 0xFFFFFFFF//todos 1 en los 32 bits.
#define SHIFT_32_BITS 31 // desplaza 31 bits

struct entero {
    //almaceno enteros de n digitos.
    //d[0] es el digito menos significativo, d[n-1] es el mas significativo.
    //solo trabajo con nros significativos.
    //d[n-1] debe ser distinto de cero.
    uint32_t *d;
    size_t n;
};

static size_t _cantidad_bits(const entero_t *e) {
    if (e == NULL || e->n == 0) return 0;

    size_t i = e->n;
    while (i > 0 && e->d[i - 1] == 0) {
        i--;
    }

    if (i == 0) return 0;

    uint32_t word = e->d[i - 1];
    size_t bits = 32 * (i - 1);

    for (int j = 31; j >= 0; j--) {
        if (word & (1U << j)) {
            bits += j + 1;
            break;
        }
    }

    return bits;
}

entero_t * _crear(size_t n){
    entero_t *entero = malloc(sizeof(entero_t));
    if(entero == NULL)return NULL;

    entero->d = malloc(n * sizeof(uint32_t)); 
    if(entero->d == NULL){
        free(entero);
        return NULL;
    }

    for(size_t i = 0; i < n; i++){
        entero->d[i] = 0;
    }

    entero->n = n;
    return entero;
}

void entero_destruir(entero_t * entero){     
    free(entero->d);    
    free(entero);
}

entero_t * entero_clonar(const entero_t * entero){
    size_t tamano = entero->n;
    entero_t *clon = _crear(tamano);
    if(clon == NULL)return NULL;

    for(size_t i = 0; i < tamano; i++){
        clon->d[i] = entero->d[i];
    }

    return clon;
}

bool _redimensionar(entero_t *entero, size_t n){
    if(n < entero -> n){
        return false;
    }

    uint32_t *aux = realloc(entero->d, n * sizeof(uint32_t));
    if(aux == NULL)return false;
     
    for(size_t i = entero->n; i < n; i++){
    	aux[i] = 0;
    } 

    entero->d = aux;  
    entero->n = n;
    return true;            
}

int entero_comparar(const entero_t *a, const entero_t *b) {
    size_t na = a->n;
    while(na > 0 && a->d[na - 1] == 0) na--;
    
    size_t nb = b->n;
    while(nb > 0 && b->d[nb - 1] == 0) nb--;
    
    if(na > nb) return 1;
    if(na < nb) return -1;
    
    for(size_t i = na; i > 0; i--) {
        if(a->d[i - 1] != b->d[i - 1]) {
            return a->d[i - 1] > b->d[i - 1] ? 1 : -1;
        }    
    }
    return 0;
}

bool entero_desplazar_derecha(entero_t *e){
    if(e == NULL || e->d == NULL || e->n == 0) return false; //no hay nada en la estructura

    uint32_t carry = 0;//almacena el bit que "sobra" al desplazar.
    for(size_t i = e->n; i > 0; i--){ //recorre desde el msb hasta el lsb
        uint32_t nuevo_carry = (e->d[i-1] & MASK_LSB)<<SHIFT_32_BITS; //mueve el bit lsb (q esta a la der) 32 posiciones a la izq p q quede en el msb del siguiente y sea el carry del prox
        e->d[i-1] >>= 1; //corro a derecha todos los bits
        e->d[i-1] |= carry; //deja el carry de la iteracion anterior en el msb
        carry = nuevo_carry;
    }
    return true;
}

bool entero_desplazar_izquierda(entero_t *e){
    if(e == NULL || e->d == NULL || e-> n == 0)return false;
    
    uint32_t carry = 0;
    size_t i = 0;
    for (i = 0; i < e->n; i++) {//recorre desde el lsb hasta el msb
        uint32_t nuevo_carry = (e->d[i] & MASK_MSB)>>SHIFT_32_BITS;//guarda el msb y lo guarda en lsb, sera el carry para el siguiente elemento
        e->d[i] <<= 1;//desplaza todos a la izq, el q se pierde es el q guarde en nuevo carry.
        e->d[i] |= carry; // el carry anterior entra por el LSB
        carry = nuevo_carry;
    }//i queda en e->n

 if(carry) {
        // Verificar si realmente necesito espacio extra
        if(e->d[e->n - 1] != 0) {
            if(!_redimensionar(e, e->n + 1)) return false;
            e->d[e->n - 1] = carry;  
        } else {
            e->d[e->n - 1] = carry;
        }
    }
    return true;
}

bool entero_sumar(entero_t *a, const entero_t *b){//a = a + b
    if(a == NULL||b == NULL||a->d == NULL||b->d==NULL||a->n==0||b->n==0)return false;

    size_t na = a->n, nb = b->n;
    uint64_t carry = 0;//uint 64 para q no haya overflow
    uint64_t suma = 0;
    size_t largo = (na > nb) ? na : nb;

    if(na < largo){//alargo el vector si es necesario
        if(!_redimensionar(a,  largo)) return false; 
        //nuevas posiciones de a en 0
        for(size_t i = na; i < largo; i++) {
            a->d[i] = 0;
        }
        a->n = largo; 
    }
   
    //suma
    for(size_t i = 0; i < largo; i++) {
        uint64_t termino_b = (i < nb) ? b->d[i] : 0; 
        suma = (uint64_t)a->d[i] + termino_b + carry;
        
        a->d[i] = (uint32_t)suma; // Guardo los 32 bits bajos
        carry = suma >> 32; // El carry son los bits superiores
    }
    if(carry != 0) {//redimensiono a para que entre el carry en caso de q haya
        if(!_redimensionar(a, largo + 1)) return false;
        a->d[largo] = (uint32_t)carry;
        a->n = largo + 1;
    }
    return true;
}

bool entero_restar(entero_t *a, const entero_t *b) { // a = a - b
    if(a == NULL || b == NULL || a->d == NULL || b->d == NULL || a->n == 0 || b->n == 0) return false;
    
    //no manejo resultados negativos, a siempre debe ser mayor o igual a b
    if(entero_comparar(a, b) < 0) return false;
    
    uint32_t carry = 0;
    size_t na = a->n, nb = b->n;
    uint64_t resta;

    for(size_t i = 0; i < na; i++) {
        //el termino de b es 0 si a tiene mas digitos
        uint32_t termino_b = (i < nb) ? b->d[i] : 0;
        
        resta = (uint64_t)a->d[i] - termino_b - carry;

        a->d[i] = (uint32_t)resta;//se guardan los 32 bits mas bajos
        carry = (resta >> 32) & 1;//el carry son los 32 mas altos, 1 si hay underflow
    }

    while(na > 1 && a->d[na - 1] == 0) {//saco los ceros no significativos
        na--;
    }
    a->n = na;

    return true;
}

char *entero_a_bcd(const entero_t *entero, size_t *n) {
    if (entero == NULL || n == NULL || entero->d == NULL || entero->n == 0) return NULL;

    const size_t bits_originales = entero->n * 32;
    const size_t espacio_bcd = entero->n * 3; //espacio para BCD
    entero_t *temp = entero_clonar(entero);
    if (temp == NULL) return NULL;

    if (!_redimensionar(temp, espacio_bcd)) {
        entero_destruir(temp);
        return NULL;
    }

    //Double Dabble
    for (size_t i = 0; i < bits_originales; i++) {
        // Ajuste BCD (sumar 3 si el digito > 4)
        for (size_t j = entero->n; j < temp->n; j++) {
            for (size_t shift = 0; shift < 32; shift += 4) {
                uint32_t digito = (temp->d[j] >> shift) & 0xF;
                if (digito > 4) {
                    temp->d[j] += 3 << shift;
                }
            }
        }
        
        // desplazo a la izquierda
        if (!entero_desplazar_izquierda(temp)) {
            entero_destruir(temp);
            return NULL;
        }
    }

    // digitos BCD
    const size_t max_digitos = (espacio_bcd - entero->n) * 8;
    char *digitos_bcd = malloc(max_digitos * sizeof(char));
    if (digitos_bcd == NULL) {
        entero_destruir(temp);
        return NULL;
    }

    size_t contador = 0;
    for (size_t i = entero->n; i < temp->n; i++) {
        for (size_t shift = 0; shift < 32; shift += 4) {
            digitos_bcd[contador++] = (temp->d[i] >> shift) & 0xF;
        }
    }

    // saco los ceros no significativos
    while (contador > 1 && digitos_bcd[contador - 1] == 0) {
        contador--;
    }

    entero_destruir(temp);
    *n = contador;
    return digitos_bcd;
}

entero_t *entero_desde_bcd(const char bcd[], size_t n){

    size_t bcd_espacio = ((n * 4) + 31) / 32;  // para almacenar BCD
    size_t bin_bits = (size_t)ceil(n * 3.32193);  // log2(10) ≈ 3.32193 || ceil redondea para arriba..
    size_t bin_espacio = (bin_bits + 31) / 32;  //para resultado binario
    
    //estructura con espacio para ambos
    entero_t *resultado = _crear(bin_espacio + bcd_espacio);
    if(resultado == NULL) return NULL;

    // guardo dígitos BCD en parte superior del array
    size_t contador = 0;
    for(size_t i = bin_espacio; i < resultado->n; i++) {
        resultado->d[i] = 0;  //inicializo
        for(size_t j = 0; j < 32 && contador < n; j += 4) {
            uint32_t digito = bcd[contador++] & 0xF;
            resultado->d[i] |= digito << j;
        }
    }

    //Double Dabble de BCD a binario
    for(size_t i = 0; i < bin_espacio * 32; i++) {
        entero_desplazar_derecha(resultado);
        
        // ajusto dígitos BCD (> 4 bits)
        for(size_t j = bin_espacio; j < resultado->n; j++) {
            for(size_t k = 0; k < 32; k += 4) {
                uint32_t digito = (resultado->d[j] >> k) & 0xF;
                if(digito >= 8) {
                    digito -= 3;
                    resultado->d[j] &= ~(0xF << k);  // limpio
                    resultado->d[j] |= digito << k;   // Guardo ajustado
                }
            }
        }
    }

    //redimensionp para borrar espacio BCD temporal
    _redimensionar(resultado, bin_espacio);
    
    // saco los ceros al final
    while(resultado->n > 1 && resultado->d[resultado->n-1] == 0) {
        resultado->n--;
    }
    
    return resultado;
}

entero_t *entero_dividir(const entero_t *dividendo, const entero_t *divisor, entero_t **resto) {
    if (dividendo == NULL || divisor == NULL || resto == NULL) return NULL;

    entero_t *q = _crear(1);           // Cociente
    entero_t *r = _crear(1);           // Residuo (acumulador)
    entero_t *a = entero_clonar(dividendo);  // copia de a para recorrer sus bits
    if (!q || !r || !a) {
        entero_destruir(q);
        entero_destruir(r);
        entero_destruir(a);
        return NULL;
    }

    size_t total_bits = _cantidad_bits(dividendo);
    for (size_t i = 0; i < total_bits; i++) {
        // Desplazar r a la izquierda
        entero_desplazar_izquierda(r);

        // Traer el bit actual desde 'a' (bit más significativo en esta iteración)
        size_t bit_pos = total_bits - 1 - i;
        size_t word_idx = bit_pos / 32;
        size_t bit_idx = bit_pos % 32;

        if (word_idx < a->n) {
            uint32_t bit = (a->d[word_idx] >> bit_idx) & 1;
            r->d[0] |= bit;
        }

        // Desplazar q a la izquierda
        entero_desplazar_izquierda(q);

        // Comparar r con divisor
        if (entero_comparar(r, divisor) >= 0) {
            entero_restar(r, divisor);
            q->d[0] |= 1;  // colocar 1 en LSB del cociente
        }
    }

    *resto = r;
    entero_destruir(a);
    return q;
}

static void imprimir_bcd(const char bcd[], size_t digitos) {
    if(digitos == 0) {
        printf("0\n");
        return;
    }
    //imprimo digitos al reves (del mas significativo al menos)
    for(size_t i = 0; i < digitos; i++) {
        size_t indice = digitos - 1 - i;
       
        //separo de a miles
        if(i != 0 && (digitos - i) % 3 == 0) {
            printf("_");  // _ como separador
        }
        
        printf("%d", bcd[indice]);
    }
    printf("\n");
}

bool entero_imprimir(const entero_t *entero){
	size_t n;
	char *bcd = entero_a_bcd(entero, &n);
	if(bcd == NULL) return false;
	imprimir_bcd(bcd, n);
	free(bcd);
	return true;
}

/*void _imprimir(const entero_t *e, const char *pre, const char *post) {
    if(e == NULL || e->d == NULL) {
        fprintf(stderr, "Error: entero invalido\n");
        return;
    }
    printf("%s", pre ? pre : "");
    //imprimo al reves
    for(size_t i = e->n; i > 0; i--) {
        printf(" %08x", e->d[i - 1]);  // 8 digitos hex con ceros a la izquierda
    }
    printf("%s\n", post ? post : "");
}*/

char *leer_bcd(size_t *n) {
    if (n == NULL) return NULL; 

    int c;
    size_t capacidad = 16;  //para reducir reallocs
    size_t i = 0;
    char *bcd = malloc(capacidad * sizeof(char));
    
    if (bcd == NULL) return NULL;

    while ((c = getchar()) != EOF && c != '\n') {
        if (c >= '0' && c <= '9') {
            // redimenciono si necesario
            if (i >= capacidad) {
                capacidad *= 2;
                char *aux = realloc(bcd, capacidad * sizeof(char));
                if (aux == NULL) {
                    free(bcd);
                    return NULL;
                }
                bcd = aux;
            }       
            bcd[i++] = c - '0';
        }
    }
    //reduzco el tamaño final
    if (i > 0 && i < capacidad / 4) {
        char *aux = realloc(bcd, i * sizeof(char));
        if (aux != NULL) {
            bcd = aux;
        }
    }
    *n = i;
    for(size_t j = 0; j < (i/2); j++){
        char temp = bcd[j];
        bcd[j] = bcd[i - j - 1];
        bcd[i - j - 1] = temp;
    }
    return bcd;
}

