#ifndef MATRIZ_H
#define MATRIZ_H
    #include <stdbool.h>
    
    char*** criaMatriz();
    void deletaMatriz(char****, int);
    void realocacao(char****, int*);
    void populaMatriz(char***, int*, FILE*, int);
    bool duplicidadeEstacoes(char***, int*, char*);
    bool duplicidadeParesEstacao(char***, int*, char*, char*);

#endif