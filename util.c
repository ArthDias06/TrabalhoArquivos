#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include<string.h>

// Convertemos um string para int, seguindo a ideia de que string nula ou vazia deve valer -1. Nota: se for inserida uma string que não é um número, o retorno é 0
int converterStringParaInt(char *str) {
    if ((strcmp(str,"") == 0) || str == NULL || (strcmp(str,"NULO") == 0)) {
        return -1;
    }
    return atoi(str);
}