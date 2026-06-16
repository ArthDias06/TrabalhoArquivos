#ifndef CRUD_H
#define CRUD_H

#include "crud_sequencial.h"
#include "crud_arvoreb.h"

// Esse arquivo foi feito mais para servir para funções específicas do CRUD que são utilizadas por mais de uma estrutura
// Na implementação atual, apenas duas função se encaixam, mas a utilidade desse .c pode aumentar conforme o projeto escala

void removerSequencial(FILE *, FILE *, CABECALHO *, ARVOREB_CABECALHO *, int, char *[][2], char ***, int *);
bool buscaSequencial(FILE *, char *[][2], int, bool);

#endif
