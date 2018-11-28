#if !defined(ROTULOS_H_)
#define ROTULOS_H_

typedef struct rotulo Rotulo;

struct rotulo{
    int rotulo;
    float distancia;
};

int cmpfunc(const void *a, const void *b);

void printa_rotulo(Rotulo r);

#endif // ROTULOS_H_
