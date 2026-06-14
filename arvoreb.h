#ifndef ARVOREB_H
#define ARVOREB_H

#define ordem 4
#define maxchaves (ordem-1)
#define minchaves (ordem/2-1)

#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<stdint.h>
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

bool lerCabecalhoArvore(FILE *fArvore, ARVOREB_CABECALHO *cabecalho);
void escreverCabecalhoArvore(FILE *fArvore, ARVOREB_CABECALHO cabecalho);
void removeChaveArvore(FILE *fArvore, ARVOREB_CABECALHO *cabecalho, int chave);
void criaRaiz(FILE* fbin, int promover, int promoverChave, int promoverRegistro, ARVOREB_CABECALHO *cabecalho);
void createIndex(char* arq, char* arvore_arq);
int busca(FILE *fbin, int chave, int RRN);
PAGINA localizaNo(FILE *fbin, int RRN, int chave, int *posicao, bool *achou);
int buscaChave(FILE *fbin, int chave, int RRN);
PAGINA ins_in_page(int chave, int registro, int filho, PAGINA pagina);
void escreverNO(FILE *fArvore, int rrn, PAGINA pagina);
PAGINA inicializaPagina();
PAGINA split(FILE* fbin, int chave, int filho, int registro, PAGINA *pagina, int *promover_chave, PAGINA novaPagina, int *promover, int *promoverRegistro, ARVOREB_CABECALHO *cabecalho);
bool insert(FILE* fbin, int registro, int RRN, int chave, int *promover, int *promover_chave, int *promoverRegistro, ARVOREB_CABECALHO *cabecalho);
PAGINA lerNO(FILE *fArvore, int rrn);
bool isFolha(PAGINA pagina);
void liberaPagina(FILE *fArvore, ARVOREB_CABECALHO *cabecalho, int rrn);
void buscaSucessor(FILE *fArvore, int rrn, int *chave, int *p);
void removeChaveFolha(PAGINA *pagina, int posicao);
void redistribui(FILE *fArvore, int rrnPai, int separacao);
void concatena(FILE *fArvore, ARVOREB_CABECALHO *cabecalho, int rrnPai, int separacao);
void trataUnderflow(FILE *fArvore, ARVOREB_CABECALHO *cabecalho, int rrnPai, int posFilho);
bool removeChaveArvoreInterno(FILE *fArvore, ARVOREB_CABECALHO *cabecalho, int rrn, int chave);
bool lerCabecalhoDados(FILE *fbin, CABECALHO *cab);
void escreverCabecalhoDados(FILE *fbin, CABECALHO cab);
void executaRemocoes(FILE *fbin, FILE *fArvore, CABECALHO *cabDados, ARVOREB_CABECALHO *cabArvore, int n);
void delete(char *arquivoBin, char *arquivoArvore, int n);

#endif
