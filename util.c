#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include<string.h>

int converterStringParaInt(char *str) {
    if ((strcmp(str,"") == 0) || str == NULL || (strcmp(str,"NULO") == 0)) {
        return -1;
    }
    return atoi(str);
}