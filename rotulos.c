#include <stdio.h>
#include "rotulos.h"

int cmpfunc(const void *a, const void *b){ // Compara dois rótulos
    Rotulo *a_ = (Rotulo *)a;
    Rotulo *b_ = (Rotulo *)b;

    float d1 = a_->distancia;
    float d2 = b_->distancia;

    return d1 > d2;
}

void printa_rotulo(Rotulo r){
    printf("Distancia: %f\n", r.distancia);
    printf("Grupo pertencente (rotulo): %d\n", r.rotulo);
}

