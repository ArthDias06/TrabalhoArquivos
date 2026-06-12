#include<stdio.h>
#include<stdlib.h>
#include "fornecidas.h"
#define ordem 4
#define maxchaves ordem-1
#define minchaves ordem/2-1

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


void criaRaiz(FILE* fbin, int promover, int promoverChave, ARVOREB_CABECALHO *cabecalho){
    PAGINA pagina = inicializaPagina();
    int rrn = cabecalho->noRaiz;
    int filho, tipoNo;
    pagina.tipoNo = 0;//Nó raiz
    pagina.chave[0] = promoverChave;
    pagina.filho[0] = cabecalho->noRaiz;
    pagina.filho[1] = promover;
    pagina.nroChaves++;
    cabecalho->noRaiz = cabecalho->proxRRN;
    fseek(fbin, 54+53*rrn, SEEK_SET);
    fread(&filho, sizeof(int), 1, fbin);
    fseek(fbin, -36, SEEK_CUR);
    if(filho != -1){
        tipoNo = 1
    }
    else tipoNo = -1
    //Atualiza o tipo no do antigo nó raiz no arquivo
    fwrite(&tipoNo, sizeof(int), 1, fbin);
    escreverNO(cabecalho->proxRRN, pagina);
    cabecalho->proxRRN++;
}


void createIndex(char* arq, char* arvore_arq){
    FILE *fbin;
    if (bin == NULL || !(fbin = fopen(bin, "rb"))) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }
    FILE *fbin_arvore;
    if (csv == NULL || !(fbin_arvore = fopen(arvore_arq, "wb"))) {
        printf("Falha no processamento do arquivo.\n");
        fclose(fbin);
        return;
    }
    ARVOREB_CABECALHO cabecalho = {'1', -1, -1, 0, 0};
    fseek(fbin, 0, SEEK_SET);
    char status, removido;
    fread(&status, sizeof(char), 1, fbin);
    if(status == '0'){
        printf("Erro no processamento do arquivo!");
        return;
    }
    int rrn=-1;
    fseek(fbin, 16, SEEK_CUR);
    while(fread(&removido, sizeof(char), 1, fbin) == 1){
        //Armazena a posição so registro
        rrn++;
        if(removido == '1') {
            fseek(fbin,79,SEEK_CUR);
            continue;
        }
        //Ignora o campo próximo
        fseek(fbin, 4, SEEK_CUR);
        int chave, promover, promoverChave;
        //Chave tem o valor de codEstacao do registro
        fread(&chave, sizeof(int), 1, fbin);
        //Insere na árvore B
        if(insert(fbin_arvore, rrn, cabecalho.noRaiz, chave, &promover, &promoverChave)){
            criaRaiz(fbin_arvore, promover, promoverChave, &cabecalho);
        }
        //Passa para o próximo registro
        fseek(fbin, 71, SEEK_CUR);
    }
    fclose(fbin_arvore);
    fclose(fbin);
    BinarioNaTela(arvore_arq);
}


int busca(FILE *fbin, int chave, int RRN){
    if(RRN = -1){
        printf("Registro não encontrado!");
        return -1;
    }
    if(fbin == NULL){
        printf("Erro no processamento do arquivo!");
        return -1;
    }
    ARVOREB_CABECALHO cabecalho;
    PAGINA pagina;
    fread(&cabecalho.status, sizeof(char), 1, fbin);
    if(cabecalho.status == '0'){
        printf("Erro no processamento do arquivo!");
        return -1;
    }
    int atual;
    //Primeiro: vai para as chaves do nó raiz, portanto
    //o primerio RRN tem que ser o do nó raiz.
    fseek(fbin, 17+53*RRN, SEEK_SET);
    fread(&pagina.removido, sizeof(char), 1, fbin);
    if(pagina.removido = '0'){
        printf("Registro não encontrado!");
        return -1;
    }
    fseek(fbin, 12, SEEK_CUR);
    //Procura pela chave
    for(int i = 0; i < 3; i++){
        fread(&atual, sizeof(int), 1, fbin);
        if(atual == chave){
            fread(&RRN, sizeof(int), 1, fbin);
            return RRN;
        }
        //pula para a próxima chave no nó
        fseek(fbin, sizeof(int), SEEK_CUR);
        if(atual > chave){
            fseek(fbin, 8*(2-i)+4*(i+1), SEEK_CUR);
            fread(&RRN, sizeof(int), 1, fbin);
            return busca(fbin, chave, RRN);
        }
    }
    //Se não foi encontrado no for, está depois da última chave
    //Então vai para o último descendente
    fseek(fbin, 12, SEEK_CUR);
    fread(&RRN, sizeof(int), 1, fbin);
    //Continua a busca no próximo filho
    return busca(fbin, chave, RRN);
}

bool procuraNo(int chave, PAGINA pagina, int *posicao){
    for(int i = 0; i < pagina.nroChaves && chave > pagina.chave[i]; i++);
    *posicao = i;
    if(*posicao < pagina.nroChaves && chave == pagina.chave[*posicao]){
        return true;
    }
    return false;
}

PAGINA ins_in_page(int chave, int filho, PAGINA pagina){
    for(int i = pagina.nroChaves-1; chave < pagina.chave[i-1] && i>0; i--){
        pagina.chave[i] = pagina.chave[i-1];
        pagina.filho[i+1] = pagina.filho[i];
    }
    pagina.nroChaves++;
    pagina.chave[i] = chave;
    pagina.filho[i+1] = filho;
    return pagina;
}


void escreverNO(int RRN, PAGINA pagina){
    fseek(fbin, 17+53*RRN, SEEK_SET);
    fwrite(&pagina.removido, sizeof(char), 1, fbin);
    fwrite(&pagina.proximo, sizeof(int), 1, fbin);
    fwrite(&pagina.tipoNo, sizeof(int), 1, fbin);
    fwrite(&pagina.nroChaves, sizeof(int), 1, fbin);
    for(int i = 0; i < maxchaves; i++){
        fwrite(&pagina.chave[i], sizeof(int), 1, fbin);
        fwrite(&pagina.registro[i], sizeof(int), 1, fbin);
    }
    for(int i = 0; i < ordem; i++){
        fwrite(&pagina.filho[i], sizeof(int), 1, fbin);
    }
}


PAGINA inicializaPagina(PAGINA pagina){
    for(int j = 0; j<maxchaves; j++){
        pagina.chave[j] = -1;
        pagina.registro[j] = -1;
        pagina.filho[j] = -1;
    }
    pagina.filho[maxchaves] = -1;
    pagina.proximo = -1;
    pagina.tipoNo = -1;
    pagina.removido = '0';
    return pagina;
}

PAGINA split(int chave, int filho, PAGINA pagina, int *promover_chave, PAGINA novaPagina, int *promover, ARVOREB_CABECALHO cabecalho){
    int chavesTrabalho[ordem];
    int filhosTrabalho[ordem+1];
    
    for(int i = 0; i < maxchaves; i++){
        chavesTrabalho[i] = pagina.chave[i];
        filhosTrabalho[i] = pagina.filho[i];
    }
    filhosTrabalho[i] = pagina.filho[i];
    for(int i = maxchaves; chave < chavesTrabalho[i-1] && i > 0; i--){
        chavesTrabalho[i] = chavesTrabalho[i-1];
        filhosTrabalho[i+1] = filhosTrabalho[i];
    }
    chavesTrabalho[i] = chave;
    filhosTrabalho[i+1] = filho;
    *promover = cabecalho.proxRRN;
    novaPagina = inicializaPagina(novaPagina);
    for(int i = 0; i < maxchaves-minchaves; i++){
        pagina.chave[i] = chavesTrabalho[i];
        pagina.filho[i] = filhosTrabalho[i];
        if(i < minchaves){
            novaPagina.chave[i] = chavesTrabalho[i + 1 + majorCount];
            novaPagina.filho[i] = filhosTrabalho[i + 1 + majorCount];
        }
        pagina.chave[i+minchaves] = -1;
        pagina.filho[i+1+minchaves] = -1;
    }
    pagina.filho[minchaves] = filhosTrabalho[minchaves];
    novaPagina.filho[minchaves] = filhosTrabalho[maxchaves+1];
    novaPagina.nroChaves = minchaves;
    pagina.nroChaves = maxchaves-minchaves;
    *promover_chave = chavesTrabalho[maxchaves-minchaves];
    return novaPagina;
}

bool insert(FILE* fbin, int registro, int RRN, int chave, int *promover, int *promover_chave, ARVOREB_CABECALHO *cabecalho){
    if(fbin == NULL){
        printf("Erro no processamento do arquivo!");
        return false;
    }
    if(cabecalho->status = '0'){
        printf("Erro no processamento do arquivo!");
        return false;
    }


    PAGINA pagina, novaPagina;
    bool encontrado, promovido;
    int posicao, filho, chave_abaixo;
    if(RRN == -1){
        *promover_chave = chave;
        *promover = -1;
        return true;
    }
    fseek(fbin, 19+53*RRN, SEEK_SET);
    fread(&pagina.tipoNo, sizeof(int), 1, fbin);
    fread(&pagina.nroChaves, sizeof(int), 1, fbin);
    for(int i = 0; i < maxchaves; i++){
        fread(&pagina.chave[i], sizeof(int), 1, fbin);
        fread(&pagina.registro[i], sizeof(int), 1, fbin);
    }
    for(int i = 0; i < ordem; i++){
        fread(&pagina.filho[i], sizeof(int), 1, fbin);
    }
    
    if(procuraNo(chave, pagina, &posicao)){
        printf("Chave já cadastrada!");
        return false;
    }

    promovido = insert(fbin, registro, pagina.filho[posicao], chave, &filho, &chave_abaixo);
    if(!promovido){
        return false;
    }
    if(pagina.nroChaves < maxchaves){
        pagina = ins_in_page(chave_abaixo, filho, pagina);
        escreverNO(RRN, pagina);
        cabecalho->proxRRN++;
        return false;
    }
    else{
        novaPagina = split(chave_abaixo, filho, pagina, promover_chave, novaPagina, promover, cabecalho);
        escreverNO(RRN, pagina);
        escreverNO(*promover , novaPagina);
        cabecalho->proxRRN+=2;
        return true;
    }
}