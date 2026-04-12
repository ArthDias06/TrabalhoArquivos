#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "registro.h"

void atualizaCabecalho(CABECALHO cabecalho, FILE* fbin){
    if(fbin == NULL){
        printf("Falha no processamento do arquivo");
        return;
    }
    fseek(fbin, 0, SEEK_SET);
    fwrite(&cabecalho.status, sizeof(char), 1, fbin);
    fwrite(&cabecalho.topo, sizeof(int), 1, fbin);
    fwrite(&cabecalho.proxRRN, sizeof(int), 1, fbin);
    fwrite(&cabecalho.nroEstacoes, sizeof(int), 1, fbin);
    fwrite(&cabecalho.nroParesEstacao, sizeof(int), 1, fbin);
}

CABECALHO leituraCabecalho(FILE* fbin){
    CABECALHO cabecalho;
    fseek(fbin,0,SEEK_SET);
    cabecalho.status = '0';
    fwrite(&cabecalho.status, sizeof(char), 1, fbin);
    fread(&cabecalho.topo, sizeof(int), 1, fbin);
    fread(&cabecalho.proxRRN, sizeof(int), 1, fbin);
    fread(&cabecalho.nroEstacoes, sizeof(int), 1,fbin);
    fread(&cabecalho.nroParesEstacao, sizeof(int), 1,fbin);
    return cabecalho;
}

void escritaRegistro(REGISTRO registro, int proxEscrita, FILE* fbin){
    fseek(fbin, proxEscrita, SEEK_SET);
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