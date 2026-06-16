#ifndef CRUD_ARVOREB_H
#define CRUD_ARVOREB_H

#include "arvoreb.h"
#include "matriz.h"

// Esse arquivo contém as funções de operações do CRUD aplicadas na árvore B

bool createIndex(char *, char *);
bool deleteFromWhereArvore(char *, char *, int, char ***, int *);
void selectFromWhereArvore(char *, char *, int);

#endif
