#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "fornecidas.h"

#include <stdint.h>
#include "registro.h"

//Função para atualizar o cabecalho de um arquivo ao final da escrita dele
void atualizarCabecalho(CABECALHO cabecalho, FILE* fbin){
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
bool lerCabecalho(FILE* fbin, CABECALHO *cabecalho){
    fseek(fbin,0,SEEK_SET);
    //Leitura de cada campo individualmente
    fread(&cabecalho->status, sizeof(char), 1, fbin);
    if(cabecalho->status == '0'){
        return false;
    }
    cabecalho->status = '0';
    fseek(fbin, -1, SEEK_CUR);
    fwrite(&cabecalho->status, sizeof(char), 1, fbin);
    fread(&cabecalho->topo, sizeof(int), 1, fbin);
    fread(&cabecalho->proxRRN, sizeof(int), 1, fbin);
    fread(&cabecalho->nroEstacoes, sizeof(int), 1,fbin);
    fread(&cabecalho->nroParesEstacao, sizeof(int), 1,fbin);
    return true;
}

//Função para escrita do registro
void escreverRegistro(REGISTRO registro, FILE* fbin){
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

// Função para ler um as condições de busca para os registros. Essa função lê o "WHERE" da cláusula
void lerCondicoesBusca(int quantAnds, char *condicoes[][2]) {
    for(int j = 0; j<quantAnds; ++j) {
        char nomeCampo[23];
        scanf(" %s", nomeCampo);
        if(strcmp(nomeCampo, "nomeEstacao")==0) {
            condicoes[j][0] = (char *)0;
        } else if(strcmp(nomeCampo, "nomeLinha")==0) {
            condicoes[j][0] = (char *)1;
        } else if(strcmp(nomeCampo, "codEstacao")==0) {
            condicoes[j][0] = (char *)2;
        } else if(strcmp(nomeCampo, "codLinha")==0) {
            condicoes[j][0] = (char *)3;
        } else if(strcmp(nomeCampo, "codProxEstacao")==0) {
            condicoes[j][0] = (char *)4;
        } else if(strcmp(nomeCampo, "distProxEstacao")==0) {
            condicoes[j][0] = (char *)5;
        } else if(strcmp(nomeCampo, "codLinhaIntegra")==0) {
            condicoes[j][0] = (char *)6;
        } else if(strcmp(nomeCampo, "codEstIntegra")==0) {
            condicoes[j][0] = (char *)7;
        }
        char valorCampo[46];
        if(strcmp(nomeCampo, "nomeEstacao") == 0 || strcmp(nomeCampo, "nomeLinha") == 0) {
            ScanQuoteString(valorCampo);
            if(strcmp(valorCampo, "") == 0) {
                strcpy(valorCampo, "NULO");
            }
        } else {
            scanf(" %s", valorCampo);
        }
        if(!strcmp(valorCampo, "")) {
            strcpy(valorCampo, "NULO");
        }
        condicoes[j][1] = malloc(strlen(valorCampo)+1);
        strcpy(condicoes[j][1], valorCampo);
    }
}

void liberarCondicoes(int quantAnds, char *condicoes[][2]) {
    for(int j=0; j<quantAnds; ++j) {
        free(condicoes[j][1]);
    }
}

void finalizarBusca(FILE *fbin, int *cont) {
    ++*cont;
    fseek(fbin, tamHeader + (*cont * tamRegistro), SEEK_SET);
}

bool lerRegistroVerifica(FILE *fbin, REGISTRO *registro, int quantAnds, char *condicoes[][2]) {
    fread(&registro->proximo, sizeof(int), 1, fbin);
    fread(&registro->codEstacao, sizeof(int), 1, fbin);
    fread(&registro->codLinha, sizeof(int), 1, fbin);
    fread(&registro->codProxEstacao, sizeof(int), 1, fbin);
    fread(&registro->distProxEstacao, sizeof(int), 1, fbin);
    fread(&registro->codLinhaIntegra, sizeof(int), 1, fbin);
    fread(&registro->codEstIntegra, sizeof(int), 1, fbin);

    fread(&registro->tamNomeEstacao, sizeof(int), 1, fbin);
    if(registro->tamNomeEstacao > 0) {
        fread(registro->nomeEstacao, sizeof(char), registro->tamNomeEstacao, fbin);
    }
    registro->nomeEstacao[registro->tamNomeEstacao] = '\0';

    fread(&registro->tamNomeLinha, sizeof(int), 1, fbin);
    if(registro->tamNomeLinha > 0) {
        fread(registro->nomeLinha, sizeof(char), registro->tamNomeLinha, fbin);
    }
    registro->nomeLinha[registro->tamNomeLinha] = '\0';


    bool ok = 1;
    for(int j=0; j<quantAnds && ok; ++j) {
        int op = (int)(intptr_t)condicoes[j][0];
        char *valorString = condicoes[j][1];
        bool nulo = strcmp(valorString, "NULO") == 0;
        int valorInt = nulo ? -1 : atoi(valorString);

        switch(op) {
            //nomeEstacao
            case 0: 
                if(nulo && registro->tamNomeEstacao != 0) {
                    ok = 0;
                }
                else if(!nulo && strcmp(registro->nomeEstacao, valorString) != 0) {
                    ok = 0;
                }
                break;
            //nomeLinha
            case 1:
                if(nulo && registro->tamNomeLinha != 0) {
                    ok = 0;
                }
                else if(!nulo && strcmp(registro->nomeLinha, valorString) != 0) {
                    ok = 0;
                }
                break;
            // codEstacao
            case 2: 
                if(registro->codEstacao != valorInt) {
                    ok = 0;
                }
                break;
            // codLinha
            case 3: 
                if(registro->codLinha != valorInt) {
                    ok = 0;
                }
                break;
            // codProxEstacao
            case 4: 
                if(registro->codProxEstacao != valorInt) {
                    ok = 0;
                }
                break;
            // distProxEstacao
            case 5: 
                if(registro->distProxEstacao != valorInt) {
                    ok = 0;
                }
                break;
            // codLinhaIntegra
            case 6: 
                if(registro->codLinhaIntegra != valorInt) {
                    ok = 0;
                }
                break;
            // codEstIntegra
            case 7:
                if(registro->codEstIntegra != valorInt) {
                    ok = 0;
                }
                break;
        }
    }
    return ok;
}

void imprimeRegistro(REGISTRO registro) {
    printf("%d ", registro.codEstacao);
    printf("%s ", registro.nomeEstacao);
    if(registro.codLinha == -1) {
        printf("NULO ");
    } else {
        printf("%d ", registro.codLinha);
    }
    if(registro.tamNomeLinha == 0) {
        printf("NULO ");
    } else {
        printf("%s ", registro.nomeLinha);
    }
    if(registro.codProxEstacao == -1) {
        printf("NULO ");
    } else {
        printf("%d ", registro.codProxEstacao);
    }
    if(registro.distProxEstacao == -1) {
        printf("NULO ");
    } else {
        printf("%d ", registro.distProxEstacao);
    }
    if(registro.codLinhaIntegra == -1) {
        printf("NULO ");
    } else {
        printf("%d ", registro.codLinhaIntegra);
    }
    if(registro.codEstIntegra == -1) {
        printf("NULO\n");
    } else {
        printf("%d\n", registro.codEstIntegra);
    }
}

bool buscaSequencial(FILE *fbin, char *condicoes[][2], int quantAnds, bool pararNoPrimeiro) {
    bool encontrou = false;
    int cont = 0;
    fseek(fbin, tamHeader, SEEK_SET);
    char removido;
    while(fread(&removido, 1, 1, fbin) == 1) {
        if(removido == '1') {
            ++cont;
            fseek(fbin, tamRegistro - 1, SEEK_CUR);
            continue;
        }
        REGISTRO registro;
        registro.removido = '0';
        bool ok = lerRegistroVerifica(fbin, &registro, quantAnds, condicoes);
        if(ok) {
            encontrou = true;
            imprimeRegistro(registro);
            if(pararNoPrimeiro) {
                break;
            }
        }
        finalizarBusca(fbin, &cont);
    }
    return encontrou;
}