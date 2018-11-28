#include <math.h>
#include <stdio.h>

float euclid_helper(float p1, float p2){ // Funçao auxiliar para 
    return (p1 - p2) * (p1 - p2);        // distância euclidiana
}

float euclid(float *p1, float *p2, int tam){ // Cálculo da distância 
    float d = 0;                             // por Euclidiana
    for (int i = 0; i < tam; i++)
        d += euclid_helper(p1[i], p2[i]);
    d = sqrt(d);
    return d;
}

float m_helper(float p1, float p2, float R){ // Funçao auxiliar para
    float res = p1 - p2;                     // distância de Minkowsky
    if (res < 0) res *= -1.0;
    return pow(res, R);
}

float m_dist(float *p1, float *p2, int tam, float R){ //Cálculo da 
    float d = 0;                                      // distância
    for (int i = 0; i < tam; i++)                     // por Minkowsky
        d += m_helper(p1[i], p2[i], R);

    d = pow(d, (1.0/R));
    return d;
}

float c_helper(float p1, float p2){ // Funcao auxiliar para
    float res = p1 - p2;            // distância de Chebyshev
    if (res < 0) res *= -1;
    return res;
}

float c_dist(float *p1, float *p2, int tam){ // Cálculo da distância
    float d = c_helper(p1[0], p2[0]);        // por Chebyshev
    for (int i = 1; i < tam; i++){
        float tmp = c_helper(p1[i], p2[i]);
        if (tmp > d) d = tmp;
    }
    return d;
}