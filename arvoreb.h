#ifndef ARVOREB_H
#define ARVOREB_H

#define ordem 4
#define maxchaves ordem-1
#define minchaves ordem/2-1

#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include"registro.h"    
#include "fornecidas.h"

typedef struct cabecalho{
    char status;
    int noRaiz;
    int topo;
    int proxRRN;
    int nroNos;
}ARVOREB_CABECALHO;

typedef struct pagina{
    int chave[maxchaves]; //codEstacao
    int filho[ordem];
    int registro[maxchaves]; //Referência à posição do registro no arquivo de dados
    char removido;
    int proximo;
    int tipoNo;
    int nroChaves;
}PAGINA;

bool lerCabecalhoArvore(FILE *fArvore, ARVOREB_CABECALHO *cabecalho);
void escreverCabecalhoArvore(FILE *fArvore, ARVOREB_CABECALHO cabecalho);
int busca(FILE *fArvore, int noRaiz, int chave);
void removeChaveArvore(FILE *fArvore, ARVOREB_CABECALHO *cabecalho, int chave);

#endif
