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
void removeChaveArvore(FILE *, ARVOREB_CABECALHO *, int);
void criaRaiz(FILE *, int, int, int, ARVOREB_CABECALHO *);
bool createIndex(char *, char *);
int busca(FILE *, int, int);
PAGINA localizaNo(FILE *, int, int, int *, bool *);
int buscaChave(FILE *, int, int);
PAGINA ins_in_page(int, int, int, PAGINA);
void escreverNO(FILE *, int, PAGINA);
PAGINA inicializaPagina();
PAGINA split(FILE *, int, int, int, PAGINA *, int *, PAGINA, int *, int *, ARVOREB_CABECALHO *);
bool insert(FILE *, int, int, int, int *, int *, int *, ARVOREB_CABECALHO *, bool *);
PAGINA lerNO(FILE *, int);
bool isFolha(PAGINA);
void liberaPagina(FILE *, ARVOREB_CABECALHO *, int);
void buscaSucessor(FILE *, int, int *, int *);
void removeChaveFolha(PAGINA *, int);
void redistribui(FILE *, int, int);
void concatena(FILE *, ARVOREB_CABECALHO *, int, int);
void trataUnderflow(FILE *, ARVOREB_CABECALHO *, int, int);
bool removeChaveArvoreInterno(FILE *, ARVOREB_CABECALHO *, int, int);
bool lerCabecalhoDados(FILE *, CABECALHO *);
void escreverCabecalhoDados(FILE *, CABECALHO);
void executaRemocoes(FILE *, FILE *, CABECALHO *, ARVOREB_CABECALHO *, int, char ***, int *);
void deleteFromWhereArvore(char *, char *, int, char ***, int *);
void selectFromWhereArvore(char *, char *, int);
void removerSequencial(FILE *, FILE *, CABECALHO *, ARVOREB_CABECALHO *, int, char *[][2], char ***, int *);
#endif
