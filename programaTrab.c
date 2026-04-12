//Arthur de Catsro Dias - 16855302
//Gabriel Carraro Salzedas - 16827905
#define tamRegistro 80

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include "fornecidas.h"
#include "registro.h"
#include "matriz.h"

bool createTable(char* csv, char* bin, char*** matrizes, int* nroLinhas){
    FILE *fbin;
    if (bin == NULL || !(fbin = fopen(bin, "wb"))) {
        printf("Falha no processamento do arquivo.\n");
        return false;
    }
    FILE *fcsv;
    if (csv == NULL || !(fcsv = fopen(csv, "r"))) {
        printf("Falha no processamento do arquivo.\n");
        fclose(fbin);
        return false;
    }
    //Valores iniciais do cabecalho
    CABECALHO cabecalho = {'0', -1, 0, 0, 0};
    fseek(fcsv, 101, SEEK_SET); //Ignora a primeira linha do CSV.
    //44 é a maior string que pode ser inserida de acordo com a especificação do projeto+\0
    //11 é a representação em string do maior valor de int + \0
    char codLin[11], nomeLin[44], distanciaProx[11], codLinInteg[11], codEstInteg[11];
    

    *nroLinhas = 0;
    int contVariavel = 0;
    REGISTRO registro;
    //Colocando valores iniciais do cabecalho
    atualizaCabecalho(cabecalho, fbin);
    int ch = 0;
    int cont = 0;


    while(ch != EOF){
        //Lê cada letra do documento
        ch = fgetc(fcsv);
        //Se for \r ignora o byte
        if(ch == '\r'){
            continue;
        }
        //contaVariavel norteia qual campo está sendo lido
        switch(contVariavel){
            //Os 7 primeiros campos são terminados por vígula
            //O último pode ser terminado por \n ou EOF
            //Caso o final do campo seja tingido é colocado \0 no fim da strin lida
            case 0:
                matrizes[1][*nroLinhas][cont] = ch!=',' ? ch : '\0';
                break;
            case 1:
                matrizes[0][*nroLinhas][cont] = ch!=',' ? ch : '\0';
                break;
            case 2:
                codLin[cont] = ch!=',' ? ch : '\0';
                break;
            case 3:
                nomeLin[cont] = ch!=',' ? ch : '\0';
                break;
            case 4:
                matrizes[2][*nroLinhas][cont] = ch!=',' ? ch : '\0';
                break;
            case 5:
                distanciaProx[cont] = ch!=',' ? ch : '\0';
                break;
            case 6:
                codLinInteg[cont] = ch!=','? ch : '\0';
                break;
            case 7:
                codEstInteg[cont] = (ch!='\n' && ch != EOF) ? ch : '\0';
                break;
        }
        //Conatdor para passar para próximo byte da string
        cont++;
        if(ch != '\n' && ch != EOF){
            if(ch == ','){
                //Se chegou ao final de um campo que não é o final
                //De uma linha, contVariavel aumenta em 1, mostrando que deveria ler o próximo campo
                contVariavel++;
                cont = 0;
            }
            continue;
        }
        //Se chegar no final da linha o registro é escrito no arquivo
        cont = 0;
        contVariavel = 0;
        //O registro acabou de ser criado então seu campo removido e proximo são por padrão 0 e -1
        registro.removido = '0';
        registro.proximo = -1;
        /*Em cada campo é feita a verificação se o valor inserido é diferente de nulo
        Com esceção dos campos codEstacao e nomeEstacao que não podem ser nulos.
        Caso algum dos campos seja nulo, seu valor passa a ser -1*/
        registro.codEstacao = atoi(matrizes[1][*nroLinhas]);
        if(!strlen(codLin)){
            registro.codLinha = -1;
        }else{
            registro.codLinha = atoi(codLin);
        }
        if(!strlen(matrizes[2][*nroLinhas])){
            registro.codProxEstacao = -1;
        }else{
            registro.codProxEstacao = atoi(matrizes[2][*nroLinhas]);
        }
        if(!strlen(distanciaProx)){
            registro.distProxEstacao = -1;
        }else{
            registro.distProxEstacao = atoi(distanciaProx);
        }
        if(!strlen(codLinInteg)){
            registro.codLinhaIntegra = -1;
        }else{
            registro.codLinhaIntegra = atoi(codLinInteg);
        }
        if(!strlen(codEstInteg)){
            registro.codEstIntegra = -1;
        }else{
            registro.codEstIntegra = atoi(codEstInteg);
        }
        registro.tamNomeEstacao = strlen(matrizes[0][*nroLinhas]);
        registro.tamNomeLinha = strlen(nomeLin);
        strcpy(registro.nomeEstacao, matrizes[0][*nroLinhas]);
        strcpy(registro.nomeLinha, nomeLin);

        //Chamad da função para escrita no registro
        escritaRegistro(registro, cabecalho.proxRRN*tamRegistro+17, fbin);
        //Aumenta o número do próximo RRN
        cabecalho.proxRRN++;


        //Caso o nome da estação já exista, o nroEstacoes não sofre alteração
        cabecalho.nroEstacoes++;cabecalho.nroParesEstacao++;
        if(duplicidadeEstacoes(matrizes, nroLinhas, matrizes[0][*nroLinhas])){
            cabecalho.nroEstacoes--;
        }
        //O mesmo para caso o par já exista
        if(duplicidadeParesEstacao(matrizes, nroLinhas, matrizes[2][*nroLinhas], matrizes[1][*nroLinhas])){
            cabecalho.nroParesEstacao--;
        }
        //Vê se o i está na última linha da matriz da matriz, se estiver, aloca mais memória
        if((*nroLinhas)% 200 == 0 && (*nroLinhas) > 0){
            realocacao(&matrizes, nroLinhas);
        }
        //Aumenta o número de linhas usadas pela matriz
        (*nroLinhas)++;
        
    }
    //Atualiza o valor do cabecalho
    cabecalho.status = '1';
    atualizaCabecalho(cabecalho, fbin);
    //Fecha as streams
    fclose (fcsv);
    fclose(fbin);
    return true;
}


void insertInto(char* arquivoBin, int nroInsert, char*** matrizes, int* nroLinhas){
    FILE *fbin;
    if(arquivoBin == NULL || !(fbin = fopen(arquivoBin, "r+b"))){
        printf("Erro no Insert!");
        return;
    }
    //Strings dos campos usados, os nomeEstacoa e nomeLinha tem 2 byes a mais por conat das aspas do input
    char codEstacao[11], nomeEstacao[46], codLinha[11], nomeLinha[46], codProxEstacao[11], distProxEstacao[11], codLinhaIntegra[11], codEstIntegra[11];
    REGISTRO registro;
    //Leitura do cabecalho do arquivo
    CABECALHO cabecalho = leituraCabecalho(fbin);
    int proxInsercao;
    //Caso a matriz não tenha sido ainda preenchida com os valores do arquivo
    if(*nroLinhas <= 0){
        populaMatriz(matrizes, nroLinhas, fbin, cabecalho.proxRRN);
    }
    //O loop ocorre de acordo com quantas inserções o usuário pretende fazer
    for(int i = 0; i < nroInsert; i++){
        //O proxInsercao marca onde será inserido o próximo registro
        //podendo ser no final do arquivo ou seguir a pilha de remoções
        //Caso haja registros deletados.
        proxInsercao = cabecalho.topo != -1 ? cabecalho.topo : cabecalho.proxRRN;
        ScanQuoteString(codEstacao);
        ScanQuoteString(nomeEstacao);
        ScanQuoteString(codLinha);
        ScanQuoteString(nomeLinha);
        ScanQuoteString(codProxEstacao);
        ScanQuoteString(distProxEstacao);
        ScanQuoteString(codLinhaIntegra);
        ScanQuoteString(codEstIntegra);
        //Verificação se a entrada do susário é válida
        if(!strcmp(codEstacao, "") || !strcmp(nomeEstacao, "")){
            printf("Os 2 primeiros campos não podem ser nulos!\n");
            continue;
        }
        //Preenchimento da variável registro com os valores lidos
        registro.codEstacao = atoi(codEstacao);
        strcpy(registro.nomeEstacao, nomeEstacao);
        strcpy(registro.nomeLinha, nomeLinha);
        if(!strcmp("", codLinha)){
            registro.codLinha = -1;
        }else{
            registro.codLinha = atoi(codLinha);
        }
        if(!strcmp("", codProxEstacao)){
            registro.codProxEstacao = -1;
        }else{
            registro.codProxEstacao = atoi(codProxEstacao);
        }
        if(!strcmp("", distProxEstacao)){
            registro.distProxEstacao = -1;
        }else{
            registro.distProxEstacao = atoi(distProxEstacao);
        }
        if(!strcmp("", codLinhaIntegra)){
            registro.codLinhaIntegra = -1;
        }else{
            registro.codLinhaIntegra = atoi(codLinhaIntegra);
        }
        if(!strcmp("", codEstIntegra)){
            registro.codEstIntegra = -1;
        }else{
            registro.codEstIntegra = atoi(codEstIntegra);
        }
        registro.tamNomeEstacao = strlen(nomeEstacao);
        registro.tamNomeLinha = strlen(nomeLinha);
        registro.removido = '0';
        registro.proximo = -1;

        //Se será adicionado no final aumenta o número de proxRRN
        if(cabecalho.proxRRN == proxInsercao){
            cabecalho.proxRRN++;
        }
        //Caso contrário desemilha o vaalor da pilha
        else{
            fseek(fbin, cabecalho.topo*tamRegistro+18,SEEK_SET);
            //Lê o campo próximo do registro removido e substitui no topo do cabecalho
            fread(&cabecalho.topo, sizeof(int), 1, fbin);
        }
        //Escrita do registro no arquivo
        escritaRegistro(registro, proxInsercao*tamRegistro+17,fbin);

        //Caso a matriz tenha chegado no limite é alocad mais memória
        if((*nroLinhas)% 200 == 0 && (*nroLinhas) > 0){
            realocacao(&matrizes, nroLinhas);
        }

        //Verificação se o par e o nome da estação já existem
        cabecalho.nroEstacoes++;cabecalho.nroParesEstacao++;
        //Ver se precisa aumentar nroEstacoes e nroParesEstacao
        if(duplicidadeEstacoes(matrizes, nroLinhas, nomeEstacao)){
            cabecalho.nroEstacoes--;
        }
        if(duplicidadeParesEstacao(matrizes, nroLinhas, codProxEstacao, codEstacao)){
            cabecalho.nroParesEstacao--;
        }

        //O nome, codigo e código da próxima estação são escritos na matriz
        strcpy(matrizes[0][*nroLinhas], nomeEstacao);
        strcpy(matrizes[1][*nroLinhas], codEstacao);
        strcpy(matrizes[2][*nroLinhas], codProxEstacao);
        //O número de linhas aumenta
        (*nroLinhas)++;
    }
    //Atualização do cabeçalho do arquivo
    cabecalho.status = '1';
    atualizaCabecalho(cabecalho, fbin);
    //Fechamento da stream
    fclose(fbin);
}

void selectFromWhere(char *, int, bool);

void update(char *arquivoBin, int nroUpdate, char ***matrizes, int* nroLinhas){
    FILE *fbin;
    if(arquivoBin == NULL || !(fbin = fopen(arquivoBin, "r+b"))){
        printf("Erro no Update!");
        return;
    }

    CABECALHO cabecalho = leituraCabecalho(fbin);
    REGISTRO registro;

    if(*nroLinhas <= 0){
        populaMatriz(matrizes, nroLinhas, fbin, cabecalho.proxRRN);
    }


    for(int i = 0; i<nroUpdate; ++i) {
        int cont = 0;
        int quantAnds = 1;
        scanf("%d", &quantAnds);
        char *condicoes[quantAnds][2];
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
            ScanQuoteString(valorCampo);
            if(!strcmp(valorCampo, "")) {
                strcpy(valorCampo, "NULO");
            }
            condicoes[j][1] = malloc(strlen(valorCampo)+1);
            strcpy(condicoes[j][1], valorCampo);
        }

        char removido;
        int quantAndsUpdate;
        scanf("%d", &quantAndsUpdate);
        char nomeCamposUpdate[quantAndsUpdate][23];
        char valoresCamposUpdate[quantAndsUpdate][46];
        for(int j = 0; j < quantAndsUpdate; j++){
            scanf(" %s", nomeCamposUpdate[j]);
            ScanQuoteString(valoresCamposUpdate[j]);
        }
        fseek(fbin,17,SEEK_SET);
        while(fread(&removido, 1, 1, fbin) == 1){
            if(removido == '1') {
                ++cont;
                fseek(fbin,79,SEEK_CUR);
                continue;
            }
            char *nomeCampos[8]; // de 0 a 7: *nomeEstacao, *nomeLinha, *codEstacao, *codLinha, *codProxEstacao, *distProxEstacao, *codLinhaIntegra, *codEstIntegra;
            
            fseek(fbin,4,SEEK_CUR);
            int temp;
            for(int j = 2; j<8; ++j) {
                fread(&temp, 4, 1, fbin);
                nomeCampos[j] = malloc(11);
                if(temp == -1) { 
                    strcpy(nomeCampos[j], "NULO");
                } else {
                    sprintf(nomeCampos[j], "%d", temp);
                }
            }
            
            int tamNomeEstacao, tamNomeLinha;
            fread(&tamNomeEstacao, 4, 1, fbin);
            nomeCampos[0] = malloc(tamNomeEstacao+1);
            fread(nomeCampos[0], tamNomeEstacao, 1, fbin);
            nomeCampos[0][tamNomeEstacao] = '\0';
            fread(&tamNomeLinha, 4, 1, fbin);
            if(tamNomeLinha == 0) {
                nomeCampos[1] = malloc(5);
                strcpy(nomeCampos[1], "NULO");
            } else {
                nomeCampos[1] = malloc(tamNomeLinha+1);
                fread(nomeCampos[1], tamNomeLinha, 1, fbin);
                nomeCampos[1][tamNomeLinha] = '\0';
            }
            bool ok = 1;
            for(int j=0; j<quantAnds && ok; ++j) {
                int op= (int)(intptr_t)condicoes[j][0];
                if(strcmp(nomeCampos[op],condicoes[j][1]) != 0) {
                    ok = 0;
                }
            }
            
            if(ok){
                for(int j = 0; j<quantAndsUpdate; ++j) {
                    if(strcmp(nomeCamposUpdate[j], "nomeEstacao")==0) {

                        fseek(fbin, 46+cont*tamRegistro, SEEK_SET);
                        fread(&tamNomeEstacao, sizeof(int), 1, fbin);
                        char nomeEstacao[44];
                        fread(nomeEstacao, sizeof(char), tamNomeEstacao, fbin);
                        nomeEstacao[tamNomeEstacao] = '\0';
                        int contador = 0;
                        for(int k = 0; k < *nroLinhas; k++){
                            if(!strcmp(matrizes[0][k], nomeEstacao)){
                                contador++;
                                if(contador>1){
                                    break;
                                }
                            }
                            if(k+1 == *nroLinhas){
                                cabecalho.nroEstacoes--;
                            }
                        }

                        tamNomeEstacao = strlen(valoresCamposUpdate[j]);
                        strcpy(nomeCampos[0], valoresCamposUpdate[j]);
                        cabecalho.nroEstacoes++;
                        if(duplicidadeEstacoes(matrizes, nroLinhas, nomeEstacao)){
                            cabecalho.nroEstacoes--;
                        }

                        strcpy(matrizes[0][cont], valoresCamposUpdate[j]);
                    } else if(strcmp(nomeCamposUpdate[j], "nomeLinha")==0) {
                        tamNomeLinha = strlen(valoresCamposUpdate[j]);
                        strcpy(nomeCampos[1], valoresCamposUpdate[j]);
                    } else if(strcmp(nomeCamposUpdate[j], "codEstacao")==0) {
                        
                        fseek(fbin, 22+cont*tamRegistro, SEEK_SET);
                        int codEstacao;
                        fread(&codEstacao, sizeof(int), 1, fbin);
                        char codEstacao2[11];
                        if(codEstacao == -1){
                            strcpy(codEstacao2, "");
                        }else{
                            sprintf(codEstacao2, "%d", codEstacao);
                        }
                        fseek(fbin, 4, SEEK_CUR);
                        int codProxEstacao;
                        fread(&codProxEstacao, sizeof(int), 1, fbin);
                        char codProxEstacao2[11];
                        if(codProxEstacao == -1){
                            strcpy(codProxEstacao2, "");
                        }else{
                            sprintf(codProxEstacao2, "%d", codProxEstacao);
                        }
                        int contador = 0;
                        for(int k = 0; k < *nroLinhas; k++){
                            if((!strcmp(matrizes[1][k], codEstacao2) && !strcmp(matrizes[2][k], codProxEstacao2)) || (!strcmp(matrizes[2][k], codEstacao2) && !strcmp(matrizes[1][k], codProxEstacao2))){
                                contador++;
                                if(contador>1){
                                    break;
                                }
                            }
                            if(k+1 == *nroLinhas){
                                cabecalho.nroParesEstacao--;
                            }
                        }


                        cabecalho.nroEstacoes++;
                        if(duplicidadeParesEstacao(matrizes, nroLinhas, nomeCampos[4], valoresCamposUpdate[j])){
                            cabecalho.nroParesEstacao--;
                        }
                        strcpy(matrizes[1][cont], valoresCamposUpdate[j]);
                        strcpy(nomeCampos[2], valoresCamposUpdate[j]);
                    } else if(strcmp(nomeCamposUpdate[j], "codLinha")==0) {
                        strcpy(nomeCampos[3], valoresCamposUpdate[j]);

                    } else if(strcmp(nomeCamposUpdate[j], "codProxEstacao")==0) {

                        fseek(fbin, 22+cont*tamRegistro, SEEK_SET);
                        int codEstacao;
                        fread(&codEstacao, sizeof(int), 1, fbin);
                        char codEstacao2[11];
                        if(codEstacao == -1){
                            strcpy(codEstacao2, "");
                        }else{
                            sprintf(codEstacao2, "%d", codEstacao);
                        }
                        fseek(fbin, 4, SEEK_CUR);
                        int codProxEstacao;
                        fread(&codProxEstacao, sizeof(int), 1, fbin);
                        char codProxEstacao2[11];
                        if(codProxEstacao == -1){
                            strcpy(codProxEstacao2, "");
                        }else{
                            sprintf(codProxEstacao2, "%d", codProxEstacao);
                        }
                        int contador = 0;
                        for(int k = 0; k < *nroLinhas; k++){
                            if((!strcmp(matrizes[1][k], codEstacao2) && !strcmp(matrizes[2][k], codProxEstacao2)) || (!strcmp(matrizes[2][k], codEstacao2) && !strcmp(matrizes[1][k], codProxEstacao2))){
                                contador++;
                                if(contador>1){
                                    break;
                                }
                            }
                            if(k+1 == *nroLinhas){
                                cabecalho.nroParesEstacao--;
                            }
                        }

                        cabecalho.nroEstacoes++;
                        if(duplicidadeParesEstacao(matrizes, nroLinhas, nomeCampos[4], valoresCamposUpdate[j])){
                            cabecalho.nroParesEstacao--;
                        }
                        strcpy(matrizes[2][cont], valoresCamposUpdate[j]);
                        strcpy(nomeCampos[4], valoresCamposUpdate[j]);

                    } else if(strcmp(nomeCamposUpdate[j], "distProxEstacao")==0) {
                        strcpy(nomeCampos[5], valoresCamposUpdate[j]);
                    } else if(strcmp(nomeCamposUpdate[j], "codLinhaIntegra")==0) {
                        strcpy(nomeCampos[6], valoresCamposUpdate[j]);
                    } else if(strcmp(nomeCamposUpdate[j], "codEstIntegra")==0) {
                        strcpy(nomeCampos[7], valoresCamposUpdate[j]);
                    }
                }

                registro.removido = '0';
                registro.proximo = -1;
                registro.codEstacao = (!strcmp(nomeCampos[2], "NULO") || !strcmp(nomeCampos[2], "")) ? -1 : atoi(nomeCampos[2]);
                
                registro.codLinha = (!strcmp(nomeCampos[3], "NULO") || !strcmp(nomeCampos[3], "")) ? -1 : atoi(nomeCampos[3]);
                
                registro.codProxEstacao = (!strcmp(nomeCampos[4], "NULO") || !strcmp(nomeCampos[4], "")) ? -1 : atoi(nomeCampos[4]);
                
                registro.distProxEstacao = (!strcmp(nomeCampos[5], "NULO") || !strcmp(nomeCampos[5], "")) ? -1 : atoi(nomeCampos[5]);
                
                registro.codLinhaIntegra = (!strcmp(nomeCampos[6], "NULO") || !strcmp(nomeCampos[6], "")) ? -1 : atoi(nomeCampos[6]);
                
                registro.codEstIntegra = (!strcmp(nomeCampos[7], "NULO") || !strcmp(nomeCampos[7], "")) ? -1 : atoi(nomeCampos[7]);

                strcpy(registro.nomeEstacao, nomeCampos[0]);
                registro.tamNomeEstacao = strlen(registro.nomeEstacao);

                strcpy(registro.nomeLinha, nomeCampos[1]);
                registro.tamNomeLinha = strlen(registro.nomeLinha);
                
                escritaRegistro(registro, 17+cont*tamRegistro, fbin);

            }
            
            for(int j = 0; j<8; ++j) {
                free(nomeCampos[j]);
            }

            ++cont;
            fseek(fbin, 17 + (cont * 80), SEEK_SET);
        }
        for(int j=0; j<quantAnds; ++j) {
            free(condicoes[j][1]);
        }
    }
    cabecalho.status = '1';
    atualizaCabecalho(cabecalho, fbin);
    fclose(fbin);
}


bool deleteFromWhere(char *,int);



int main(){
    int operacao, nroLinhas = 0, n;
    char arquivoBin[101];
    char arquivoCSV[101];
    char*** matrizes = criaMatriz();
    while(scanf("%d", &operacao) == 1){
        switch(operacao){
            case 1:
                scanf("%100s %100s", arquivoCSV, arquivoBin);
                if(createTable(arquivoCSV, arquivoBin, matrizes, &nroLinhas))
                    BinarioNaTela(arquivoBin);
                break;
            case 2:
                scanf("%100s",arquivoBin);
                selectFromWhere(arquivoBin,1,false);
                break;
            case 3:
                int n0;
                scanf("%100s %d",arquivoBin,&n0);
                selectFromWhere(arquivoBin,n0,true);
                break;
            case 4:
                int n1;
                scanf("%100s %d",arquivoBin,&n1);
                bool erro = deleteFromWhere(arquivoBin,n1);
                if(!erro) {
                    BinarioNaTela(arquivoBin);
                }
                break;
            case 5:
                scanf("%100s %d", arquivoBin, &n);
                insertInto(arquivoBin, n, matrizes, &nroLinhas);
                BinarioNaTela(arquivoBin);
                break;
            case 6:
                scanf("%100s %d", arquivoBin, &n);
                update(arquivoBin, n, matrizes, &nroLinhas);
                BinarioNaTela(arquivoBin);
                break;
        }
    }
    //Cálculo de quanta memória foi criada para armazenar a matriz
    deletaMatriz(&matrizes, nroLinhas);
}


void selectFromWhere(char *arquivoBin, int quantBuscas, bool temWhere) {
    FILE *fbin;
    if(arquivoBin == NULL || !(fbin = fopen(arquivoBin, "rb"))){
        printf("Erro no processamento do arquivo.\n");
        return;
    }
    for(int i = 0; i<quantBuscas; ++i) {
        int cont = 0;
        int quantAnds = 1;
        if(temWhere) {
            scanf("%d", &quantAnds);
        }
        char *condicoes[quantAnds][2];
        if(temWhere) {
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
        fseek(fbin,17,SEEK_SET);
        
        bool encontrou = 0;
        char removido;
        while(fread(&removido, 1, 1, fbin) == 1){
            if(removido == '1') {
                ++cont;
                fseek(fbin,79,SEEK_CUR);
                continue;
            }
            char *nomeCampos[8]; // de 0 a 7: *nomeEstacao, *nomeLinha, *codEstacao, *codLinha, *codProxEstacao, *distProxEstacao, *codLinhaIntegra, *codEstIntegra;
            
            fseek(fbin,4,SEEK_CUR);
            int temp;
            for(int j = 2; j<8; ++j) {
                fread(&temp, 4, 1, fbin);
                nomeCampos[j] = malloc(11);
                if(temp == -1) { 
                    strcpy(nomeCampos[j], "NULO");
                } else {
                    sprintf(nomeCampos[j], "%d", temp);
                }
            }
            
            int tamNomeEstacao, tamNomeLinha;
            fread(&tamNomeEstacao, 4, 1, fbin);
            nomeCampos[0] = malloc(tamNomeEstacao+1);
            fread(nomeCampos[0], tamNomeEstacao, 1, fbin);
            nomeCampos[0][tamNomeEstacao] = '\0';
            fread(&tamNomeLinha, 4, 1, fbin);
            if(tamNomeLinha == 0) {
                nomeCampos[1] = malloc(5);
                strcpy(nomeCampos[1], "NULO");
            } else {
                nomeCampos[1] = malloc(tamNomeLinha+1);
                fread(nomeCampos[1], tamNomeLinha, 1, fbin);
                nomeCampos[1][tamNomeLinha] = '\0';
            }
            bool ok = 1;
            if(temWhere) {
                for(int j=0; j<quantAnds && ok; ++j) {
                    int op= (int)(intptr_t)condicoes[j][0];
                    if(strcmp(nomeCampos[op],condicoes[j][1]) != 0) {
                        ok = 0;
                    }

                }
            }

            if(ok) {
                encontrou = 1;
                printf("%s %s %s %s %s %s %s %s\n", nomeCampos[2], nomeCampos[0], nomeCampos[3], nomeCampos[1], nomeCampos[4], nomeCampos[5], nomeCampos[6], nomeCampos[7]);
            }
            
            for(int j = 0; j<8; ++j) {
                free(nomeCampos[j]);
            }

            ++cont;
            fseek(fbin, 17 + (cont * 80), SEEK_SET);
        }
        if(!encontrou) {
            printf("Registro inexistente.\n");
        }
        if(temWhere) {
            for(int j=0; j<quantAnds; ++j) {
                free(condicoes[j][1]);
            }
            printf("\n");
        }
    }
    fclose(fbin);
}

bool deleteFromWhere(char *arquivoBin, int quantRemocoes) {
    FILE *fbin;
    if(arquivoBin == NULL || !(fbin = fopen(arquivoBin, "r+b"))){
        printf("Falha no processamento do arquivo.");
        return 1;
    }

    CABECALHO cabecalho = leituraCabecalho(fbin);

    for(int i = 0; i<quantRemocoes; ++i) {
        int cont = 0;
        int quantAnds = 1;
        
        scanf("%d", &quantAnds);
        
        char *condicoes[quantAnds][2];
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
            
            char valorCampo[46] = {0}; 
            ScanQuoteString(valorCampo);
            if(!strcmp(valorCampo, "")) {
                strcpy(valorCampo, "NULO");
            }
            condicoes[j][1] = malloc(strlen(valorCampo)+1);
            strcpy(condicoes[j][1], valorCampo);
        }
        
        fseek(fbin, 17, SEEK_SET);

        char removido;

        while(fread(&removido, 1, 1, fbin) == 1){
            if(removido == '1') {
                fseek(fbin, 79, SEEK_CUR);
                ++cont;
                continue;
            }
            char *nomeCampos[8]; 
            
            fseek(fbin, 4, SEEK_CUR);
            int temp;
            for(int j = 2; j < 8; ++j) {
                fread(&temp, 4, 1, fbin);
                nomeCampos[j] = malloc(15);
                if(temp == -1) { 
                    strcpy(nomeCampos[j], "NULO");
                } else {
                    sprintf(nomeCampos[j], "%d", temp);
                }
            }
            
            int tamNomeEstacao, tamNomeLinha;
            fread(&tamNomeEstacao, 4, 1, fbin);
            nomeCampos[0] = malloc(tamNomeEstacao+1);
            if(tamNomeEstacao > 0) {
                fread(nomeCampos[0], tamNomeEstacao, 1, fbin);
            }
            nomeCampos[0][tamNomeEstacao] = '\0';
            
            fread(&tamNomeLinha, 4, 1, fbin);
            if(tamNomeLinha == 0) {
                nomeCampos[1] = malloc(5);
                strcpy(nomeCampos[1], "NULO");
            } else {
                nomeCampos[1] = malloc(tamNomeLinha+1);
                fread(nomeCampos[1], tamNomeLinha, 1, fbin);
                nomeCampos[1][tamNomeLinha] = '\0';
            }
            
            bool ok = 1;
            for(int j = 0; j<quantAnds && ok; ++j) {
                int op = (int)(intptr_t)condicoes[j][0];
                if(strcmp(nomeCampos[op], condicoes[j][1]) != 0) {
                    ok = 0;
                }
            }

            if(ok) {
                fseek(fbin, 17 + (cont * 80), SEEK_SET);
                
                char marcarRemocao = '1';
                fwrite(&marcarRemocao, 1, 1, fbin);
                fwrite(&cabecalho.topo, 4, 1, fbin);
                cabecalho.topo = cont;
                
                long posAtual = ftell(fbin);
                bool nomeEstacaoAindaExiste = false;
                bool parAindaExiste = false;
                
                fseek(fbin, 17, SEEK_SET);
                char removido;
                int cont = 0;
                while(fread(&removido, 1, 1, fbin) == 1) {
                    if(removido == '0') {
                        int camposInt[7];
                        fseek(fbin, 4, SEEK_CUR);
                        
                        for(int k=0; k<7; ++k) {
                            fread(&camposInt[k], 4, 1, fbin);
                        }
                        
                        int tamNome = camposInt[6];
                        char nomeEstacao[50] = {0};
                        if(tamNome > 0) {
                            fread(nomeEstacao, 1, tamNome, fbin);
                        }
                        if(strcmp(nomeEstacao, nomeCampos[0]) == 0) {
                            nomeEstacaoAindaExiste = true;
                        }
                        
                        int codEstacao = (!strcmp(nomeCampos[2], "NULO")) ? -1 : atoi(nomeCampos[2]);
                        int codProx = (!strcmp(nomeCampos[4], "NULO")) ? -1 : atoi(nomeCampos[4]);
                        
                        if(camposInt[0] == codEstacao && camposInt[2] == codProx) {
                            parAindaExiste = true;
                        }
                        if(nomeEstacaoAindaExiste && parAindaExiste) {
                            break;
                        }
                    }
                    ++cont;
                    fseek(fbin, 17 + (cont * 80), SEEK_SET);
                }
                
                
                if(!nomeEstacaoAindaExiste) {
                    --cabecalho.nroEstacoes;
                }
                if(!parAindaExiste) {
                    --cabecalho.nroParesEstacao;
                }

                fseek(fbin, posAtual, SEEK_SET);
            }
            
            for(int j = 0; j<8; ++j) {
                free(nomeCampos[j]);
            }

            ++cont;
            fseek(fbin, 17 + (cont * 80), SEEK_SET);
        }
        
        for(int j = 0; j < quantAnds; ++j) {
            free(condicoes[j][1]);
        }
    }
    
    cabecalho.status = '1';
    atualizaCabecalho(cabecalho, fbin);

    fclose(fbin);
    return 0;
}