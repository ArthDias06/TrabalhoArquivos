#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "matriz.h"

char*** criaMatriz(){
    char*** matrizes = malloc(sizeof(char**) * 3);
    for(int i = 0; i < 3; i++){
        matrizes[i] = malloc(sizeof(char*) * 200);
    }
    for(int i = 0; i < 200; i++){
        matrizes[0][i] = malloc(sizeof(char) * 44);
        matrizes[1][i] = malloc(sizeof(char) * 11);
        matrizes[2][i] = malloc(sizeof(char) * 11);
    }
    return matrizes;
}

void deletaMatriz(char**** matrizes, int nroLinhas){
    if((*matrizes) != NULL || matrizes != NULL) {
        int temp = nroLinhas/200;
        nroLinhas = !(nroLinhas%200) ? nroLinhas : ((temp+1)*200);
        for(int i = 0; i < 3; i++){
            for(int j = 0; j < nroLinhas; j++){
                free((*matrizes)[i][j]);
            }
            free((*matrizes)[i]);
        }
        free(*matrizes);
    }
}

void realocacao(char**** matrizes, int* nroLinhas){
    (*matrizes)[0] = realloc((*matrizes)[0], sizeof(char*) * ((*nroLinhas)+200));
    (*matrizes)[1] = realloc((*matrizes)[1], sizeof(char*) * ((*nroLinhas)+200));
    (*matrizes)[2] = realloc((*matrizes)[2], sizeof(char*) * ((*nroLinhas)+200));
    for(int j = *nroLinhas; j < (*nroLinhas)+200; j++){
        (*matrizes)[0][j] = malloc(sizeof(char) * 44);
        (*matrizes)[1][j] = malloc(sizeof(char) * 11);
        (*matrizes)[2][j] = malloc(sizeof(char) * 11);
    }
}

void populaMatriz(char*** matrizes, int* nroLinhas, FILE* fbin, int proxRRN){
    for(*nroLinhas = 0; *nroLinhas<proxRRN; (*nroLinhas)++){
        if((*nroLinhas)% 200 == 0 && (*nroLinhas) > 0){
            realocacao(&matrizes, nroLinhas);
        }
        int temp;
        char rem;
        fread(&rem, sizeof(char), 1, fbin);
        if(rem == '0'){
            fseek(fbin,4,SEEK_CUR);
            fread(&temp, sizeof(int), 1, fbin);
            sprintf(matrizes[1][*nroLinhas], "%d", temp);
            fseek(fbin,4,SEEK_CUR);
            fread(&temp, sizeof(int), 1, fbin);
            if(temp == -1) strcpy(matrizes[2][*nroLinhas], "");
            else sprintf(matrizes[2][*nroLinhas], "%d", temp);
            fseek(fbin,12,SEEK_CUR);
            fread(&temp, sizeof(int), 1, fbin);
            fread(matrizes[0][*nroLinhas], sizeof(char), temp, fbin);
            fseek(fbin, 17 + (*nroLinhas + 1) * 80, SEEK_SET);
            matrizes[0][*nroLinhas][temp] = '\0';
        }else{
            fseek(fbin, 17 + (*nroLinhas + 1) * 80, SEEK_SET);
            --*nroLinhas;
        }
    }
}

bool duplicidadeEstacoes(char*** matrizes, int* nroLinhas, char* nomeEstacao){
    for(int i = 0; i < *nroLinhas; i++){
        if(!strcmp(matrizes[0][i], nomeEstacao)){
            return true;
        }
    }
    return false;
}

bool duplicidadeParesEstacao(char*** matrizes, int* nroLinhas, char* codProxEstacao, char* codEstacao){
    for(int i = 0; i < *nroLinhas; i++){
        if(!strcmp(codProxEstacao, "") || (!strcmp(codEstacao, matrizes[1][i]) && !strcmp(codProxEstacao, matrizes[2][i])) || (!strcmp(codProxEstacao, matrizes[1][i]) && !strcmp(codEstacao, matrizes[2][i]))){
            return true;
        }
    }
    return false;
}