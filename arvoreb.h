#ifndef ARVOREB_H
#define ARVOREB_H

#define ordem 4
#define maxchaves (ordem-1)
#define minchaves (ordem/2-1)

#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<stdint.h>
#include<string.h>
#include"registro.h"    
#include "fornecidas.h"

// Esse arquivo contém as funções internas das operações do CRUD aplicadas na árvore B

typedef struct cabecalho_arvore{
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

bool lerCabecalhoArvore(FILE *, ARVOREB_CABECALHO *);
void escreverCabecalhoArvore(FILE *, ARVOREB_CABECALHO);
void removerChaveArvore(FILE *, ARVOREB_CABECALHO *, int);
void criaRaiz(FILE *, int, int, int, ARVOREB_CABECALHO *);
PAGINA localizaNo(FILE *, int, int, int *, bool *);
int buscarChave(FILE *, int, int);
PAGINA ins_in_page(int, int, int, PAGINA);
void escreverNO(FILE *, int, PAGINA);
PAGINA inicializaPagina();
PAGINA split(FILE *, int, int, int, PAGINA *, int *, PAGINA, int *, int *, ARVOREB_CABECALHO *);
bool insert(FILE *, int, int, int, int *, int *, int *, ARVOREB_CABECALHO *, bool *);
PAGINA lerNO(FILE *, int);
bool isFolha(PAGINA);
void liberarPagina(FILE *, ARVOREB_CABECALHO *, int);
void buscarSucessor(FILE *, int, int *, int *);
void removerChaveFolha(PAGINA *, int);
void redistribuir(FILE *, int, int);
void concatenar(FILE *, ARVOREB_CABECALHO *, int, int);
void tratarUnderflow(FILE *, ARVOREB_CABECALHO *, int, int);
bool removerChaveArvoreInterno(FILE *, ARVOREB_CABECALHO *, int, int);
#endif
