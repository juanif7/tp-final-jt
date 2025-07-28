
#include "racional.h"

racional_t entrada_a_racional(const char* entrada) {
    racional_t resultado;
    resultado.s = false; //ante cualquier cosa devolvemos +0/1
    resultado.n = 0;
    resultado.d = 1;
    
    if (entrada == NULL || strlen(entrada) == 0) {
        return resultado;
    }
    
    int inicio = 0;
    int len = strlen(entrada);
    
    // Verificar signo
    if (entrada[0] == '-') {
        resultado.s = true;
        inicio = 1;
    } else if (entrada[0] == '+') {
        inicio = 1;
    }
    
    // Buscar punto decimal
    int pos_punto = -1;
    for (int i = inicio; i < len; i++) {
        if (entrada[i] == '.') {
            pos_punto = i;
            break;
        }
    }
    
    if (pos_punto == -1) {
        // nro sin . (1234) pasa a ser 1234/1  
        entero_t numero = 0;
        for (int i = inicio; i < len; i++) {
            if (entrada[i] >= '0' && entrada[i] <= '9') {
                numero = numero * 10 + (entrada[i] - '0'); //armo el nro pasando los caracteres con sus decenas y haviendo el coso asci
            }
        }
        resultado.n = numero;
        resultado.d = 1;
        
    } else {
        // nro con .(123.4)
        // n = todos los dígitos sin punto
        // d = 10^(cantidad de dígitos después del punto)
        
        entero_t numerador = 0;
        int digitos_decimales = 0;
        
        //parte entera
        for (int i = inicio; i < pos_punto; i++) {
            if (str[i] >= '0' && str[i] <= '9') {
                numerador = numerador * 10 + (str[i] - '0');
            }
        }
        
        //parte decimal
        for (int i = pos_punto + 1; i < len; i++) {
            if (entrada[i] >= '0' && entrada[i] <= '9') {
                numerador = numerador * 10 + (entrada[i] - '0');
                digitos_decimales++;
            }
        }
        
        //denominador como potencia de 10
        entero_t denominador = 1;
        for (int i = 0; i < digitos_decimales; i++) {
            denominador *= 10;
        }

        resultado.n = numerador;
        resultado.d = denominador;
        
        // Simplificar la fracción
        resultado = simplificar_racional(resultado);
    }
    
    return resultado;
}
