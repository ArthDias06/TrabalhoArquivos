#ifndef CRUD_SEQUENCIAL_H
#define CRUD_SEQUENCIAL_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include "fornecidas.h"
#include "registro.h"
#include "matriz.h"
#include "util.h"

bool createTable(char *, char *, char ***, int *);
bool insertInto(char *, int, char ***, int *, bool, char*);
bool update(char *, int, char ***, int *);
void selectFromWhere(char *, int, bool);
bool deleteFromWhere(char *, int, char ***, int *);

#endif
