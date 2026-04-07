//Arthur de Catsro Dias - 16855302
//Gabriel Carraro Salzedas - 16827905
#define tamRegistro 80

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

typedef struct cabecalho{
    char status;
    int topo;
    int proxRRN;
    int nroEstacoes;
    int nroParesEstacao;
}CABECALHO;

typedef struct registro{
    char removido;
    int proximo;
    int codEstacao;
    int codLinha;
    int codProxEstacao;
    int distProxEstacao;
    int codLinhaIntegra;
    int codEstIntegra;
    int tamNomeEstacao;
    char nomeEstacao[44];
    int tamNomeLinha;
    char nomeLinha[44];
}REGISTRO;



int createTable(char* csv, char* bin, char**** matrizes){
    FILE *fbin;
    if (bin == NULL || !(fbin = fopen(bin, "wb"))) {
        printf("Falha no processamento do arquivo.\n");
        return -1;
    }
    FILE *fcsv;
    if (csv == NULL || !(fcsv = fopen(csv, "r"))) {
        printf("Falha no processamento do arquivo.\n");
        fclose(fbin);
        return -1;
    }
    //Começam em -1 para poderem iterarem corretamente
    CABECALHO cabecalho = {'0', -1, 0, 0, 0};
    fseek(fcsv, 101, SEEK_SET); //Ignora a primeira linha do CSV.
    char codLin[11], nomeLin[44], distanciaProx[11], codLinInteg[11], codEstInteg[11];
    *matrizes = malloc(sizeof(char**) * 3);
    for(int i = 0; i < 3; i++){
        (*matrizes)[i] = malloc(sizeof(char*) * 200);
    }
    for(int i = 0; i < 200; i++){
        (*matrizes)[0][i] = malloc(sizeof(char) * 44);
        (*matrizes)[1][i] = malloc(sizeof(char) * 11);
        (*matrizes)[2][i] = malloc(sizeof(char) * 11);
    }
    int i = 0;
    int contVariavel = 0;
    REGISTRO registro;
    //Colocando valores iniciais para ir para a inserção dos inserts
    fwrite(&cabecalho.status, sizeof(char), 1, fbin);
    fwrite(&cabecalho.topo, sizeof(int), 1, fbin);
    fwrite(&cabecalho.proxRRN, sizeof(int), 1, fbin);
    fwrite(&cabecalho.nroEstacoes, sizeof(int), 1, fbin);
    fwrite(&cabecalho.nroParesEstacao, sizeof(int), 1, fbin);
    //Lê cada letra do documento
    int ch = 0;
    int cont = 0;
    int contByte; //Conta quantos bytes foram passados para completar com lixo depois


    while(ch != EOF){
        ch = fgetc(fcsv);
        if(ch == '\r'){
            continue;
        }
        switch(contVariavel){
            case 0:
                (*matrizes)[1][i][cont] = ch!=',' ? ch : '\0';
                break;
            case 1:
                (*matrizes)[0][i][cont] = ch!=',' ? ch : '\0';
                break;
            case 2:
                codLin[cont] = ch!=',' ? ch : '\0';
                break;
            case 3:
                nomeLin[cont] = ch!=',' ? ch : '\0';
                break;
            case 4:
                (*matrizes)[2][i][cont] = ch!=',' ? ch : '\0';
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
        cont++;
        if(ch != '\n' && ch != EOF){
            if(ch == ','){
                contVariavel++;
                cont = 0;
            }
            continue;
        }
        printf("%s %s %s %s %s %s %s %s\n", (*matrizes)[1][i], (*matrizes)[0][i], codLin, nomeLin, (*matrizes)[2][i], distanciaProx, codLinInteg, codEstInteg);
        cont = 0;
        contVariavel = 0;
        //atoi de string vazia retorna 0
        registro.removido = '0';
        registro.proximo = -1;
        if(!strlen((*matrizes)[1][i])){
            registro.codEstacao = -1;
        }else{
            registro.codEstacao = atoi((*matrizes)[1][i]);
        }
        if(!strlen(codLin)){
            registro.codLinha = -1;
        }else{
            registro.codLinha = atoi(codLin);
        }
        if(!strlen((*matrizes)[2][i])){
            registro.codProxEstacao = -1;
        }else{
            registro.codProxEstacao = atoi((*matrizes)[2][i]);
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
        registro.tamNomeEstacao = strlen((*matrizes)[0][i]);
        registro.tamNomeLinha = strlen(nomeLin);
        strcpy(registro.nomeEstacao, (*matrizes)[0][i]);
        strcpy(registro.nomeLinha, nomeLin);

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
        cabecalho.proxRRN++;



        cabecalho.nroEstacoes++;cabecalho.nroParesEstacao++;
        for(int j = 0; j < i; j++){
            if(!strcmp((*matrizes)[0][j], (*matrizes)[0][i])){
                cabecalho.nroEstacoes--;
                break;
            }
        }
        for(int j = 0; j < i; j++){
            if(!strcmp((*matrizes)[1][j], (*matrizes)[1][i]) && !strcmp((*matrizes)[2][j], (*matrizes)[2][i])){
                cabecalho.nroParesEstacao--;
                break;
            }
        }
        //Vê se o i está na última linha da matriz da matriz, se estiver, aloca mais memória
        if(i% 200 == 0 && i > 0){
            (*matrizes)[0] = realloc((*matrizes)[0], sizeof(char*) * (i+200));
            (*matrizes)[1] = realloc((*matrizes)[1], sizeof(char*) * (i+200));
            (*matrizes)[2] = realloc((*matrizes)[2], sizeof(char*) * (i+200));
            for(int j = i; j < i+200; j++){
                (*matrizes)[0][j] = malloc(sizeof(char) * 44);
                (*matrizes)[1][j] = malloc(sizeof(char) * 11);
                (*matrizes)[2][j] = malloc(sizeof(char) * 11);
            }
        }
        i++;
        //Espaço para colocar lixo('$')
        contByte = 43 - strlen(registro.nomeEstacao) - strlen(registro.nomeLinha);
        if(contByte > 0){
            char lixo[contByte];
            for(int j = 0; j < contByte; j++){
                lixo[j] = '$';
            }
            fwrite(lixo, sizeof(char), contByte, fbin);
        }
    }

    cabecalho.status = '1';
    fseek(fbin, 0, SEEK_SET);
    fwrite(&cabecalho.status, sizeof(char), 1, fbin);
    fwrite(&cabecalho.topo, sizeof(int), 1, fbin);
    fwrite(&cabecalho.proxRRN, sizeof(int), 1, fbin);
    fwrite(&cabecalho.nroEstacoes, sizeof(int), 1, fbin);
    fwrite(&cabecalho.nroParesEstacao, sizeof(int), 1, fbin);

    fclose (fcsv);
    fclose(fbin);
    return i;
}






void BinarioNaTela(char *arquivo) {
    FILE *fs;
    if (arquivo == NULL || !(fs = fopen(arquivo, "rb"))) {
        fprintf(stderr,
                "ERRO AO ESCREVER O BINARIO NA TELA (função binarioNaTela): "
                "não foi possível abrir o arquivo que me passou para leitura. "
                "Ele existe e você tá passando o nome certo? Você lembrou de "
                "fechar ele com fclose depois de usar?\n");
        return;
    }

    fseek(fs, 0, SEEK_END);
    size_t fl = ftell(fs);

    fseek(fs, 0, SEEK_SET);
    unsigned char *mb = (unsigned char *)malloc(fl);
    fread(mb, 1, fl, fs);

    unsigned long cs = 0;
    for (unsigned long i = 0; i < fl; i++) {
        cs += (unsigned long)mb[i];
    }

    printf("%lf\n", (cs / (double)100));

    free(mb);
    fclose(fs);
}



void ScanQuoteString(char *str) {
    char R;

    while ((R = getchar()) != EOF && isspace(R)); // ignorar espaços, \r, \n...

    if (R == 'N' || R == 'n') { // campo NULO
        getchar();
        getchar();
        getchar();       // ignorar o "ULO" de NULO.
        strcpy(str, ""); // copia string vazia
    } else if (R == '\"') {
        if (scanf("%[^\"]", str) != 1) { // ler até o fechamento das aspas
            strcpy(str, "");
        }
        getchar();         // ignorar aspas fechando
    } else if (R != EOF) { // vc tá tentando ler uma string que não tá entre
                           // aspas! Fazer leitura normal %s então, pois deve
                           // ser algum inteiro ou algo assim...
        str[0] = R;
        scanf("%s", &str[1]);
    } else { // EOF
        printf("%s\n", str);
        strcpy(str, "");
    }
}





void insertInto(char* arquivoBin, int nroInsert, char*** matrizes, int* nroLinhas){
    FILE *fbin;
    if(arquivoBin == NULL || !(fbin = fopen(arquivoBin, "r+b"))){
        printf("Erro no Insert!");
        return;
    }
    char codEstacao[11], nomeEstacao[46], codLinha[11], nomeLinha[46], codProxEstacao[11], distProxEstacao[11], codLinhaIntegra[11], codEstIntegra[11];
    REGISTRO registro;
    CABECALHO cabecalho;
    fseek(fbin,0,SEEK_SET);
    int proxInsercao;
    //Registro aberto para escrita deve ter status como 0 - inconsistente
    cabecalho.status = '0';
    fwrite(&cabecalho.status, sizeof(char), 1, fbin);
    fread(&cabecalho.topo, sizeof(int), 1, fbin);
    fread(&cabecalho.proxRRN, sizeof(int), 1, fbin);
    fread(&cabecalho.nroEstacoes, sizeof(int), 1,fbin);
    fread(&cabecalho.nroParesEstacao, sizeof(int), 1,fbin);
    for(int i = 0; i < nroInsert; i++){
        proxInsercao = cabecalho.topo != -1 ? cabecalho.topo : cabecalho.proxRRN;
        ScanQuoteString(codEstacao);
        ScanQuoteString(nomeEstacao);
        ScanQuoteString(codLinha);
        ScanQuoteString(nomeLinha);
        ScanQuoteString(codProxEstacao);
        ScanQuoteString(distProxEstacao);
        ScanQuoteString(codLinhaIntegra);
        ScanQuoteString(codEstIntegra);
        if(!strcmp(codEstacao, "") || !strcmp(nomeEstacao, "")){
            printf("Os 2 primeiros campos não podem ser nulos!\n");
            continue;
        }
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

        cabecalho.nroEstacoes++;
        cabecalho.nroParesEstacao++;
        //Ver se precisa aumentar nroEstacoes e nroParesEstacao
        for(int i = 0; i < *nroLinhas; i++){
            if(!strcmp(matrizes[0][i], nomeEstacao)){
                cabecalho.nroEstacoes--;
                break;
            }
        }
        for(int i = 0; i < *nroLinhas; i++){
            if(!strcmp(matrizes[1][i], codEstacao) && !strcmp(matrizes[2][i], codProxEstacao)){
                cabecalho.nroParesEstacao--;
                break;
            }
        }


        //Escrita no arquivo
        fseek(fbin, proxInsercao*tamRegistro+17, SEEK_SET);
        fwrite(&registro.removido, sizeof(char),1,fbin);
        fwrite(&registro.proximo,sizeof(int),1,fbin);
        fwrite(&registro.codEstacao,sizeof(int),1,fbin);
        fwrite(&registro.codLinha,sizeof(int),1,fbin);
        fwrite(&registro.codProxEstacao,sizeof(int),1,fbin);
        fwrite(&registro.distProxEstacao,sizeof(int),1,fbin);
        fwrite(&registro.codLinhaIntegra,sizeof(int),1,fbin);
        fwrite(&registro.codEstIntegra,sizeof(int),1,fbin);
        fwrite(&registro.tamNomeEstacao,sizeof(int),1,fbin);
        fwrite(registro.nomeEstacao,sizeof(char),strlen(registro.nomeEstacao),fbin);
        fwrite(&registro.tamNomeLinha,sizeof(int),1,fbin);
        fwrite(registro.nomeLinha,sizeof(char),strlen(registro.nomeLinha),fbin);
        int contByte = 43 - strlen(registro.nomeEstacao) - strlen(registro.nomeLinha);
        char lixo[contByte];
        for(int i = 0; i < contByte; i++){
            lixo[i] = '$';
        }
        fwrite(lixo, sizeof(char), contByte, fbin);


        if(cabecalho.proxRRN == proxInsercao){
            cabecalho.proxRRN++;
        }else{
            fseek(fbin, cabecalho.topo*tamRegistro+1,SEEK_SET);
            fread(&proxInsercao, sizeof(int), 1, fbin);
            int temp = -1;
            fseek(fbin, -4, SEEK_CUR);
            //Deixa o valor -1 no próximo dele, pois não faz parte da pilha
            fwrite(&temp, sizeof(int), 1, fbin);
            cabecalho.topo = proxInsercao;
        }

        if((*nroLinhas)% 200 == 0 && (*nroLinhas) > 0){
            matrizes[0] = realloc(matrizes[0], sizeof(char*) * ((*nroLinhas)+200));
            matrizes[1] = realloc(matrizes[1], sizeof(char*) * ((*nroLinhas)+200));
            matrizes[2] = realloc(matrizes[2], sizeof(char*) * ((*nroLinhas)+200));
            for(int j = *nroLinhas; j < (*nroLinhas)+200; j++){
                matrizes[0][j] = malloc(sizeof(char) * 44);
                matrizes[1][j] = malloc(sizeof(char) * 11);
                matrizes[2][j] = malloc(sizeof(char) * 11);
            }
        }
        strcpy(matrizes[0][*nroLinhas], nomeEstacao);
        strcpy(matrizes[1][*nroLinhas], codEstacao);
        strcpy(matrizes[2][*nroLinhas], codProxEstacao);
        (*nroLinhas)++;
    }
    cabecalho.status = '1';
    fseek(fbin, 0, SEEK_SET);
    fwrite(&cabecalho.status, sizeof(char), 1, fbin);
    fwrite(&cabecalho.topo, sizeof(int), 1, fbin);
    fwrite(&cabecalho.proxRRN, sizeof(int), 1, fbin);
    fwrite(&cabecalho.nroEstacoes, sizeof(int), 1, fbin);
    fwrite(&cabecalho.nroParesEstacao, sizeof(int), 1, fbin);
    fclose(fbin);
}

void selectFromWhere(char *, int, bool);

void update(char *arquivoBin, int nroUpdate, char ***matriz, int* nroLinhas){
    FILE *fbin;
    if(arquivoBin == NULL || !(fbin = fopen(arquivoBin, "r+b"))){
        printf("Erro no Update!");
        return;
    }

    CABECALHO cabecalho;
    fseek(fbin,0,SEEK_SET);
    //Registro aberto para escrita deve ter status como 0 - inconsistente
    cabecalho.status = '0';
    fwrite(&cabecalho.status, sizeof(char), 1, fbin);
    fseek(fbin, 8, SEEK_CUR);
    fread(&cabecalho.nroEstacoes, sizeof(int), 1,fbin);
    fread(&cabecalho.nroParesEstacao, sizeof(int), 1,fbin);
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
        fseek(fbin,17,SEEK_SET);

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
            for(int j=0; j<quantAnds && ok; ++j) {
                int op= (int)(intptr_t)condicoes[j][0];
                if(strcmp(nomeCampos[op],condicoes[j][1]) != 0) {
                    ok = 0;
                }
            }
            if(ok){
                int quantAndsUpdate;
                scanf("%d", &quantAndsUpdate);
                for(int j = 0; j<quantAndsUpdate; ++j) {
                    char nomeCampo[23];
                    scanf(" %s", nomeCampo);
                    char valorCampo[46];
                    ScanQuoteString(valorCampo);
                    if(strcmp(nomeCampo, "nomeEstacao")==0) {
                        tamNomeEstacao = strlen(valorCampo);
                        strcpy(nomeCampos[0], valorCampo);
                        cabecalho.nroEstacoes++;
                        for(int k = 0; k < *nroLinhas; k++){
                            if(!strcmp(matriz[0][k], valorCampo)){
                                cabecalho.nroEstacoes--;
                                break;
                            }
                        }
                        strcpy(matriz[0][cont], valorCampo);
                    } else if(strcmp(nomeCampo, "nomeLinha")==0) {
                        tamNomeLinha = strlen(valorCampo);
                        strcpy(nomeCampos[1], valorCampo);
                    } else if(strcmp(nomeCampo, "codEstacao")==0) {
                        cabecalho.nroEstacoes++;
                        for(int k = 0; k < *nroLinhas; k++){
                            if(!strcmp(matriz[1][k], valorCampo) && !strcmp(matriz[2][k], nomeCampos[4])){
                                cabecalho.nroParesEstacao--;
                                break;
                            }
                        }
                        strcpy(matriz[1][cont], valorCampo);
                        strcpy(nomeCampos[2], valorCampo);
                    } else if(strcmp(nomeCampo, "codLinha")==0) {
                        strcpy(nomeCampos[3], valorCampo);
                    } else if(strcmp(nomeCampo, "codProxEstacao")==0) {
                        cabecalho.nroEstacoes++;
                        for(int k = 0; k < *nroLinhas; k++){
                            if(!strcmp(matriz[2][k], valorCampo) && !strcmp(matriz[1][k], nomeCampos[2])){
                                cabecalho.nroParesEstacao--;
                                break;
                            }
                        }
                        strcpy(matriz[2][cont], valorCampo);
                        strcpy(nomeCampos[4], valorCampo);
                    } else if(strcmp(nomeCampo, "distProxEstacao")==0) {
                        strcpy(nomeCampos[5], valorCampo);
                    } else if(strcmp(nomeCampo, "codLinhaIntegra")==0) {
                        strcpy(nomeCampos[6], valorCampo);
                    } else if(strcmp(nomeCampo, "codEstIntegra")==0) {
                        strcpy(nomeCampos[7], valorCampo);
                    }
                }
                fseek(fbin, 22+cont*tamRegistro, SEEK_SET);
                int codEstacao = (!strcmp(nomeCampos[2], "NULO") || !strcmp(nomeCampos[2], "")) ? -1 : atoi(nomeCampos[2]);
                fwrite(&codEstacao, sizeof(int), 1, fbin);
                int codLinha = (!strcmp(nomeCampos[3], "NULO") || !strcmp(nomeCampos[3], "")) ? -1 : atoi(nomeCampos[3]);
                fwrite(&codLinha, sizeof(int), 1, fbin);
                int codProxEstacao = (!strcmp(nomeCampos[4], "NULO") || !strcmp(nomeCampos[4], "")) ? -1 : atoi(nomeCampos[4]);
                fwrite(&codProxEstacao, sizeof(int), 1, fbin);
                int distProxEstacao = (!strcmp(nomeCampos[5], "NULO") || !strcmp(nomeCampos[5], "")) ? -1 : atoi(nomeCampos[5]);
                fwrite(&distProxEstacao, sizeof(int), 1, fbin);
                int codLinhaIntegra = (!strcmp(nomeCampos[6], "NULO") || !strcmp(nomeCampos[6], "")) ? -1 : atoi(nomeCampos[6]);
                fwrite(&codLinhaIntegra, sizeof(int), 1, fbin);
                int codEstIntegra = (!strcmp(nomeCampos[7], "NULO") || !strcmp(nomeCampos[7], "")) ? -1 : atoi(nomeCampos[7]);
                fwrite(&codEstIntegra, sizeof(int), 1, fbin);

                fwrite(&tamNomeEstacao, sizeof(int), 1, fbin);
                fwrite(nomeCampos[0], sizeof(char), tamNomeEstacao, fbin);
                fwrite(&tamNomeLinha, sizeof(int), 1, fbin);
                fwrite(nomeCampos[1], sizeof(char), tamNomeLinha, fbin);
                
                int contByte = 43 - tamNomeEstacao - tamNomeLinha;
                char lixo[contByte];
                for(int j = 0; j < contByte; j++){
                    lixo[j] = '$';
                }
                fwrite(lixo, sizeof(char), contByte, fbin);
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
    fseek(fbin, 0, SEEK_SET);
    fwrite(&cabecalho.status, sizeof(char), 1, fbin);
    fseek(fbin, 8, SEEK_CUR);
    fwrite(&cabecalho.nroEstacoes, sizeof(int), 2, fbin);
    fwrite(&cabecalho.nroParesEstacao, sizeof(int), 1, fbin);
    fclose(fbin);
}


bool deleteFromWhere(char *,int);



int main(){
    int operacao, nroLinhas = 0, n;
    char arquivoBin[101];
    char arquivoCSV[101];
    char*** matrizes = NULL;
    while(scanf("%d", &operacao) == 1){
        switch(operacao){
            case 1:
                scanf("%100s %100s", arquivoCSV, arquivoBin);
                nroLinhas = createTable(arquivoCSV, arquivoBin, &matrizes);
                if(nroLinhas != -1) {
                    BinarioNaTela(arquivoBin);
                }
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
                break;
        }
    }
    //Cálculo de quanta memória foi criada para armazenar a matriz
    if(matrizes != NULL) {
        int temp = nroLinhas/200;
        nroLinhas = !(nroLinhas%200) ? nroLinhas : ((temp+1)*200);
        for(int i = 0; i < 3; i++){
            for(int j = 0; j < nroLinhas; j++){
                free(matrizes[i][j]);
            }
            free(matrizes[i]);
        }
        free(matrizes);
    }
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

    CABECALHO cabecalho;
    fseek(fbin, 0, SEEK_SET);
    
    fread(&cabecalho.status, sizeof(char), 1, fbin);
    fread(&cabecalho.topo, sizeof(int), 1, fbin);
    fread(&cabecalho.proxRRN, sizeof(int), 1, fbin);
    fread(&cabecalho.nroEstacoes, sizeof(int), 1, fbin);
    fread(&cabecalho.nroParesEstacao, sizeof(int), 1, fbin);

    fseek(fbin, 0, SEEK_SET);
    //Registro aberto para escrita deve ter status como 0 - inconsistente
    cabecalho.status = '0';
    fwrite(&cabecalho.status, sizeof(char), 1, fbin);

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
    fseek(fbin, 0, SEEK_SET);
    fwrite(&cabecalho.status, sizeof(char), 1, fbin);
    fwrite(&cabecalho.topo, sizeof(int), 1, fbin);
    fwrite(&cabecalho.proxRRN, sizeof(int), 1, fbin);
    fwrite(&cabecalho.nroEstacoes, sizeof(int), 1, fbin);
    fwrite(&cabecalho.nroParesEstacao, sizeof(int), 1, fbin);

    fclose(fbin);
    return 0;
}