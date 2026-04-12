#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "matriz.h"

/* A matriz de de 3 dimensões é usada no trabalho
para conferir a repetição de estações e de pares de estações.
O conjunto 0 da matriz é uma matriz bidimensional de strings, contendo o
nome das estações.
O conjunto 1 e 2 possuem o código de cada estação e o código dqa próxima estação respectivamente.
*/

//Criação da matriz
char*** criaMatriz(){
    char*** matrizes = malloc(sizeof(char**) * 3);
    //Inicialmente ela pode receber até 200 registros
    for(int i = 0; i < 3; i++){
        matrizes[i] = malloc(sizeof(char*) * 200);
    }
    for(int i = 0; i < 200; i++){
        matrizes[0][i] = malloc(sizeof(char) * 44);//44 é o maior nome que a estação pode ter+\0
        matrizes[1][i] = malloc(sizeof(char) * 11);//11 é o número de algarismos que o maior int  pode ter+\0
        matrizes[2][i] = malloc(sizeof(char) * 11);
    }
    return matrizes;
}

//Função para deleção da matriz
void deletaMatriz(char**** matrizes, int nroLinhas){
    if((*matrizes) != NULL || matrizes != NULL) {
        int temp = nroLinhas/200;
        //Caso nroLinhas seja múltiplo de 200 ele simplesmente 
        //Percorre a matriz nroLinhas vezes, mas caso contrário
        //Ele deve percorrer o mmúltiplo de 200 maior e mais próximo de temp
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

//Caso a matriz atinja um múltiplo de 200 ela deve ser realocad e mais 200 espaçoes são liberados para ela usar
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

//Em casos que o createTable não é chamado, a matriz precisa ter os dados do arquivo existente.
void populaMatriz(char*** matrizes, int* nroLinhas, FILE* fbin, int proxRRN){
    *nroLinhas = 0;
    // Começamos no 1° registro
    fseek(fbin,17,SEEK_SET);
    //Percorre todo o arquivo
    for(int i = 0; i<proxRRN; ++i) {
        //Se tiveer alcançado o limite da matriz aumenta o espaço permitido
        if((*nroLinhas)% 200 == 0 && (*nroLinhas) > 0){
            realocacao(&matrizes, nroLinhas);
        }
        int temp;
        char rem;
        fread(&rem, sizeof(char), 1, fbin);
        //Se o registro existir
        if(rem == '0'){
            fseek(fbin,4,SEEK_CUR);
            //Lê o campo codEstacao
            fread(&temp, sizeof(int), 1, fbin);
            sprintf(matrizes[1][*nroLinhas], "%d", temp);
            fseek(fbin,4,SEEK_CUR);
            //Lê o codProxEstacao
            fread(&temp, sizeof(int), 1, fbin);
            //Se o código for -1 preenche com string vazia
            if(temp == -1){ 
                strcpy(matrizes[2][*nroLinhas], "");
            }
            //Caso contrário converte o valor lido em string
            else {
                sprintf(matrizes[2][*nroLinhas], "%d", temp);
            }
            fseek(fbin,12,SEEK_CUR);
            //Lê o tamNomeEstacao
            fread(&temp, sizeof(int), 1, fbin);
            //Lê o nomeEstacao
            fread(matrizes[0][*nroLinhas], sizeof(char), temp, fbin);
            //Finaliza a string com \0
            matrizes[0][*nroLinhas][temp] = '\0';
            //Vai para o próximo registro
            ++(*nroLinhas);
        }
        fseek(fbin, 17 + (i+1) * 80, SEEK_SET);
    }
}

//Função para conferir se o nomeEstaacao já existe na matriz
bool duplicidadeEstacoes(char*** matrizes, int* nroLinhas, char* nomeEstacao){
    for(int i = 0; i < *nroLinhas; i++){
        if(!strcmp(matrizes[0][i], nomeEstacao)){
            return true;
        }
    }
    return false;
}

//Função para verificar se o par já existe na matriz
bool duplicidadeParesEstacao(char*** matrizes, int* nroLinhas, char* codProxEstacao, char* codEstacao){
    for(int i = 0; i < *nroLinhas; i++){
        /*São feitas as seguintes verificacoes:
        1-Se o codProxEstacao é nulo
        1-Se o codEstacao está presente no conjunto 1 da matriz e se o codProxEstacao está no conjunto 2 da matriz
        2-Se o codEstacao está presente no conjunto 2 da matriz e se o codProxEstacao está no conjunto 1 da matriz*/
        if((!strcmp(codEstacao, matrizes[1][i]) && !strcmp(codProxEstacao, matrizes[2][i])) || (!strcmp(codProxEstacao, matrizes[1][i]) && !strcmp(codEstacao, matrizes[2][i]))){
            return true;
        }
    }
    return false;
}