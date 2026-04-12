#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "registro.h"

//Função para atualizar o cabecalho de um arquivo ao final da escrita dele
void atualizaCabecalho(CABECALHO cabecalho, FILE* fbin){
    if(fbin == NULL){
        printf("Falha no processamento do arquivo");
        return;
    }
    fseek(fbin, 0, SEEK_SET);
    //Escreve cada campo do cabecalho individualmente
    fwrite(&cabecalho.status, sizeof(char), 1, fbin);
    fwrite(&cabecalho.topo, sizeof(int), 1, fbin);
    fwrite(&cabecalho.proxRRN, sizeof(int), 1, fbin);
    fwrite(&cabecalho.nroEstacoes, sizeof(int), 1, fbin);
    fwrite(&cabecalho.nroParesEstacao, sizeof(int), 1, fbin);
}

//Função para ler o cabecalho de um dado arquivo
CABECALHO leituraCabecalho(FILE* fbin){
    CABECALHO cabecalho;
    fseek(fbin,0,SEEK_SET);
    //Comoas funções de busca não usam do cebcalho, ele só é chamado em funções de escrita
    //Por conta disso aqui o status é definido como 0(inconsistente)
    cabecalho.status = '0';
    //Leitura de cada campo individualmente
    fwrite(&cabecalho.status, sizeof(char), 1, fbin);
    fread(&cabecalho.topo, sizeof(int), 1, fbin);
    fread(&cabecalho.proxRRN, sizeof(int), 1, fbin);
    fread(&cabecalho.nroEstacoes, sizeof(int), 1,fbin);
    fread(&cabecalho.nroParesEstacao, sizeof(int), 1,fbin);
    //Retorno do cabecalho lido
    return cabecalho;
}

//Função para escrita do registro
void escritaRegistro(REGISTRO registro, int proxEscrita, FILE* fbin){
    //Define o ponteiro na região da próximainserção
    fseek(fbin, proxEscrita, SEEK_SET);
    //Escreve tudo campo a campo
    fwrite(&registro.removido, sizeof(char), 1, fbin);
    fwrite(&registro.proximo, sizeof(int), 1, fbin);
    fwrite(&registro.codEstacao, sizeof(int), 1, fbin);
    fwrite(&registro.codLinha, sizeof(int), 1, fbin);
    fwrite(&registro.codProxEstacao, sizeof(int), 1, fbin);
    fwrite(&registro.distProxEstacao, sizeof(int), 1, fbin);
    fwrite(&registro.codLinhaIntegra, sizeof(int), 1, fbin);
    fwrite(&registro.codEstIntegra, sizeof(int), 1, fbin);
    fwrite(&registro.tamNomeEstacao, sizeof(int), 1, fbin);
    fwrite(registro.nomeEstacao, sizeof(char), strlen(registro.nomeEstacao), fbin);
    fwrite(&registro.tamNomeLinha, sizeof(int), 1, fbin);
    fwrite(registro.nomeLinha, sizeof(char), strlen(registro.nomeLinha), fbin);
    //No final é preenchido o restante do registro com o lixo representado por $
    int contByte = 43 - strlen(registro.nomeEstacao) - strlen(registro.nomeLinha);
    if(contByte > 0){
        char lixo[contByte+1];
        for(int j = 0; j < contByte; j++){
            lixo[j] = '$';
        }
        lixo[contByte] = '\0';
        fwrite(lixo, sizeof(char), contByte, fbin);
    }
}