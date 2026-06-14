#ifndef CRUD_H
#include<stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include "fornecidas.h"
#include "registro.h"
#include "matriz.h"
#include "util.h"
#include "arvoreb.h"

#define CRUD_H

    bool createTable(char *, char *, char ***, int *);
    void insertInto(char *, int,  char ***, int *, bool, char*);
    void update(char *, int, char ***, int *);
    void selectFromWhere(char *, int, bool);
    bool deleteFromWhere(char *,int, char ***, int *);

#endif